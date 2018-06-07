#include <algorithm>
#include <mutex>

#include "components/util/atomic_pointer.h"
#include "components/util/env.h"
#include "components/util/random.h"

#include "thirdparty/glog/logging.h"
#include "thirdparty/gtest/gtest.h"

static const int kDelayMicros = 100000;
static const int kReadOnlyFileLimit = 4;
static const int kMMapLimit = 4;

class EnvTest : public testing::Test {
public:
    Env* env_;
    EnvTest() : env_(Env::Default()) { }
};

static void SetBool(void* ptr) {
    reinterpret_cast<AtomicPointer*>(ptr)->NoBarrier_Store(ptr);
}

Slice RandomString(Random* rnd, int len, std::string* dst) {
    dst->resize(len);
    for (int i = 0; i < len; i++) {
        (*dst)[i] = static_cast<char>(' ' + rnd->Uniform(95));   // ' ' .. '~'
    }
    return Slice(*dst);
}

TEST_F(EnvTest, ReadWrite) {
    Random rnd(301);

    // Get file to use for testing.
    std::string test_dir;
    ASSERT_TRUE(env_->GetTestDirectory(&test_dir).IsOk());
    std::string test_file_name = test_dir + "/open_on_read.txt";
    WritableFile* writable_file;
    ASSERT_TRUE(env_->NewWritableFile(test_file_name, &writable_file).IsOk());

    // Fill a file with data generated via a sequence of randomly sized writes.
    static const size_t kDataSize = 10 * 1048576;
    std::string data;
    while (data.size() < kDataSize) {
        int len = rnd.Skewed(18);  // Up to 2^18 - 1, but typically much smaller
        std::string r;
        RandomString(&rnd, len, &r);
        ASSERT_TRUE(writable_file->Append(r).IsOk());
        data += r;
        if (rnd.OneIn(10)) {
            ASSERT_TRUE(writable_file->Flush().IsOk());
        }
    }
    ASSERT_TRUE(writable_file->Sync().IsOk());
    ASSERT_TRUE(writable_file->Close().IsOk());
    delete writable_file;

    // Read all data using a sequence of randomly sized reads.
    SequentialFile* sequential_file;
    ASSERT_TRUE(env_->NewSequentialFile(test_file_name, &sequential_file).IsOk());
    std::string read_result;
    std::string scratch;
    while (read_result.size() < data.size()) {
        int len = std::min<int>(rnd.Skewed(18), data.size() - read_result.size());
        scratch.resize(std::max(len, 1));  // at least 1 so &scratch[0] is legal
        Slice read;
        ASSERT_TRUE(sequential_file->Read(len, &read, &scratch[0]).IsOk());
        if (len > 0) {
            ASSERT_GT(read.Size(), 0);
        }
        ASSERT_LE(read.Size(), len);
        read_result.append(read.Data(), read.Size());
    }
    ASSERT_EQ(read_result, data);
    delete sequential_file;
}

TEST_F(EnvTest, RunImmediately) {
    AtomicPointer called(nullptr);
    env_->Schedule(&SetBool, &called);
    env_->SleepForMicroseconds(kDelayMicros);
    ASSERT_TRUE(called.NoBarrier_Load() != nullptr);
}

TEST_F(EnvTest, RunMany) {
    AtomicPointer last_id(nullptr);

    struct CB {
        AtomicPointer* last_id_ptr;   // Pointer to shared slot
        uintptr_t id;             // Order# for the execution of this callback
 
        CB(AtomicPointer* p, int i) : last_id_ptr(p), id(i) { }

        static void Run(void* v) {
            CB* cb = reinterpret_cast<CB*>(v);
            void* cur = cb->last_id_ptr->NoBarrier_Load();
            ASSERT_EQ(cb->id-1, reinterpret_cast<uintptr_t>(cur));
            cb->last_id_ptr->Release_Store(reinterpret_cast<void*>(cb->id));
        }
    };

    // Schedule in different order than start time
    CB cb1(&last_id, 1);
    CB cb2(&last_id, 2);
    CB cb3(&last_id, 3);
    CB cb4(&last_id, 4);
    env_->Schedule(&CB::Run, &cb1);
    env_->Schedule(&CB::Run, &cb2);
    env_->Schedule(&CB::Run, &cb3);
    env_->Schedule(&CB::Run, &cb4);
    
    env_->SleepForMicroseconds(kDelayMicros);
    void* cur = last_id.Acquire_Load();
    ASSERT_EQ(4, reinterpret_cast<uintptr_t>(cur));
}
    
struct State {
    std::mutex mu;
    int val;
    int num_running;

    State(int val, int num_running) : val(val), num_running(num_running) { }
};

static void ThreadBody(void* arg) {
    State* s = reinterpret_cast<State*>(arg);
    s->mu.lock();
    s->val += 1;
    s->num_running -= 1;
    s->mu.unlock();
}

TEST_F(EnvTest, StartThread) {
    State state(0, 3);
    for (int i = 0; i < 3; i++) {
        env_->StartThread(&ThreadBody, &state);
    }
    while (true) {
        state.mu.lock();
        int num = state.num_running;
        state.mu.unlock();
        if (num == 0) {
            break;
        }
        env_->SleepForMicroseconds(kDelayMicros);
    }

    std::unique_lock<std::mutex> l(state.mu);
    ASSERT_EQ(state.val, 3);
}

TEST_F(EnvTest, TestOpenNonExistentFile) {
    // Write some test data to a single file that will be opened |n| times.
    std::string test_dir;
    ASSERT_TRUE(env_->GetTestDirectory(&test_dir).IsOk());

    std::string non_existent_file = test_dir + "/non_existent_file";
    ASSERT_TRUE(!env_->FileExists(non_existent_file));

    RandomAccessFile* random_access_file;
    Status status = env_->NewRandomAccessFile(
        non_existent_file, &random_access_file);
    ASSERT_TRUE(status.IsNotFound());

    SequentialFile* sequential_file;
    status = env_->NewSequentialFile(non_existent_file, &sequential_file);
    ASSERT_TRUE(status.IsNotFound());
}

TEST_F(EnvTest, ReopenWritableFile) {
    std::string test_dir;
    ASSERT_TRUE(env_->GetTestDirectory(&test_dir).IsOk());
    std::string test_file_name = test_dir + "/reopen_writable_file.txt";
    env_->DeleteFile(test_file_name);

    WritableFile* writable_file;
    ASSERT_TRUE(env_->NewWritableFile(test_file_name, &writable_file).IsOk());
    std::string data("hello world!");
    ASSERT_TRUE(writable_file->Append(data).IsOk());
    ASSERT_TRUE(writable_file->Close().IsOk());
    delete writable_file;

    ASSERT_TRUE(env_->NewWritableFile(test_file_name, &writable_file).IsOk());
    data = "42";
    ASSERT_TRUE(writable_file->Append(data).IsOk());
    ASSERT_TRUE(writable_file->Close().IsOk());
    delete writable_file;

    ASSERT_TRUE(ReadFileToString(env_, test_file_name, &data).IsOk());
    ASSERT_EQ(std::string("42"), data);
    env_->DeleteFile(test_file_name);
}

TEST_F(EnvTest, ReopenAppendableFile) {
    std::string test_dir;
    ASSERT_TRUE(env_->GetTestDirectory(&test_dir).IsOk());
    std::string test_file_name = test_dir + "/reopen_appendable_file.txt";
    env_->DeleteFile(test_file_name);

    WritableFile* appendable_file;
    ASSERT_TRUE(env_->NewAppendableFile(test_file_name, &appendable_file).IsOk());
    std::string data("hello world!");
    ASSERT_TRUE(appendable_file->Append(data).IsOk());
    ASSERT_TRUE(appendable_file->Close().IsOk());
    delete appendable_file;

    ASSERT_TRUE(env_->NewAppendableFile(test_file_name, &appendable_file).IsOk());
    data = "42";
    ASSERT_TRUE(appendable_file->Append(data).IsOk());
    ASSERT_TRUE(appendable_file->Close().IsOk());
    delete appendable_file;

    ASSERT_TRUE(ReadFileToString(env_, test_file_name, &data).IsOk());
    ASSERT_EQ(std::string("hello world!42"), data);
    env_->DeleteFile(test_file_name);
}
