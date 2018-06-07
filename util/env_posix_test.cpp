#include "components/util/env.h"
#include "components/util/env_posix_test_helper.h"

#include "thirdparty/glog/logging.h"
#include "thirdparty/gtest/gtest.h"

static const int kDelayMicros = 100000;
static const int kReadOnlyFileLimit = 4;
static const int kMMapLimit = 4;

class EnvPosixTest : public testing::Test {
public:
    Env* env_;
    EnvPosixTest() : env_(Env::Default()) { }

    static void SetFileLimits(int read_only_file_limit, int mmap_limit) {
        EnvPosixTestHelper::SetReadOnlyFDLimit(read_only_file_limit);
        EnvPosixTestHelper::SetReadOnlyMMapLimit(mmap_limit);
    }
};

TEST_F(EnvPosixTest, TestOpenOnRead) {
    // Write some test data to a single file that will be opened |n| times.
    std::string test_dir;
    ASSERT_TRUE(env_->GetTestDirectory(&test_dir).IsOk());
    std::string test_file = test_dir + "/open_on_read.txt";

    FILE* f = fopen(test_file.c_str(), "w");
    ASSERT_TRUE(f != nullptr);
    const char kFileData[] = "abcdefghijklmnopqrstuvwxyz";
    fputs(kFileData, f);
    fclose(f);

    // Open test file some number above the sum of the two limits to force
    // open-on-read behavior of POSIX Env RandomAccessFile.
    const int kNumFiles = kReadOnlyFileLimit + kMMapLimit + 5;
    RandomAccessFile* files[kNumFiles] = {0};
    for (int i = 0; i < kNumFiles; i++) {
        ASSERT_TRUE(env_->NewRandomAccessFile(test_file, &files[i]).IsOk());
    }
    char scratch;
    Slice read_result;
    for (int i = 0; i < kNumFiles; i++) {
        ASSERT_TRUE(files[i]->Read(i, 1, &read_result, &scratch).IsOk());
        ASSERT_EQ(kFileData[i], read_result[0]);
    }
    for (int i = 0; i < kNumFiles; i++) {
        delete files[i];
    }
    ASSERT_TRUE(env_->DeleteFile(test_file).IsOk());
}
