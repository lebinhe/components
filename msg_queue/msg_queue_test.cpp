#include "components/msg_queue/msg.h"
#include "components/msg_queue/msg_queue.h"
#include "thirdparty/glog/logging.h"
#include "thirdparty/gtest/gtest.h"

#include <functional>
#include <string>
#include <thread>
#include <vector>

// Test that MsgUIDs generated in two different threads simultaneously are unique
TEST(Msg, MsgUID) {
    const int N = 1000;
    std::vector<MsgUID> uids1, uids2;

    auto create_msgs = [](int count, std::vector<MsgUID>& uids) {
        for (int i = 0; i < count; ++i)
            uids.push_back(Msg(1).GetUniqueId());
    };

    std::thread t1(create_msgs, N, std::ref(uids1));
    std::thread t2(create_msgs, N, std::ref(uids2));
    t1.join();
    t2.join();

    for (auto uid1 : uids1) {
        for (auto uid2 : uids2) {
            EXPECT_NE(uid1, uid2);
        }
    }
}

// Test that messages are received in order in 1-to-1 one-way messaging scenario
TEST(MsgQueue, MsgOrder) {
    const int N = 1000;
    MsgQueue queue;

    auto sender = [](int count, MsgQueue& q) {
        for (int i = 0; i < count; ++i)
            q.Put(Msg(i));
    };

    auto receiver = [](int count, MsgQueue& q) {
        for (int i = 0; i < count; ++i) {
            auto m = q.Get();
            EXPECT_EQ(m->GetMsgId(), i);
        }
    };

    std::thread t1(sender, N, std::ref(queue));
    std::thread t2(receiver, N, std::ref(queue));
    t1.join();
    t2.join();
}

// Test that messages are received in order in 2-to-1 one-way messaging scenario
TEST(MsgQueue, 2To1MsgOrder) {
    const int N = 1000;
    MsgQueue queue;

    auto sender = [](int msg_id, int count, MsgQueue& q) {
        for (int i = 0; i < count; ++i)
            q.Put(DataMsg<int>(msg_id, i));
    };

    auto receiver = [](int count, MsgQueue& q) {
        int expected_data1 = 0;
        int expected_data2 = 0;

        for (int i = 0; i < count; ++i) {
            auto m = q.Get();
            auto& dm = dynamic_cast<DataMsg<int>&>(*m);

            if (dm.GetMsgId() == 1) {
                EXPECT_EQ(dm.GetPayload(), expected_data1);
                ++expected_data1;
            } else if (dm.GetMsgId() == 2) {
                EXPECT_EQ(dm.GetPayload(), expected_data2);
                ++expected_data2;
            } else {
                CHECK(false) << "Unexpected message id";
            }
        }
    };

    std::thread t1(sender, 1, N, std::ref(queue));
    std::thread t2(sender, 2, N, std::ref(queue));
    std::thread t3(receiver, 2 * N, std::ref(queue));
    t1.join();
    t2.join();
    t3.join();
}

TEST(MsgQueue, DataMsg) {
    MsgQueue q;
    q.Put(DataMsg<std::string>(42, "foo"));
    auto m = q.Get();
    auto& dm = dynamic_cast<DataMsg<std::string>&>(*m);
    EXPECT_EQ(dm.GetMsgId(), 42);
    EXPECT_STREQ(dm.GetPayload().c_str(), "foo");
    // Test modifying the payload data
    dm.GetPayload() += "bar";
    EXPECT_STREQ(dm.GetPayload().c_str(), "foobar");
}

// Test timeout when getting message from the queue
TEST(MsgQueue, ReceiveTimeout) {
    MsgQueue q;

    // Test first with a Msg in the queue that specifying timeout for get() doesn't have an effect
    q.Put(Msg(1));

    auto start = std::chrono::steady_clock::now();
    auto m = q.Get(10);
    auto end = std::chrono::steady_clock::now();
    auto dur = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    EXPECT_EQ(m->GetMsgId(), 1);

    // Then test with empty queue
    start = std::chrono::steady_clock::now();
    auto m2 = q.Get(10);
    end = std::chrono::steady_clock::now();
    dur = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    EXPECT_EQ(m2->GetMsgId(), MSG_TIMEOUT);
}

// Test 2-to-1 request-response scenario
TEST(MsgQueue, RequestResponse) {
    const int N = 1000;

    MsgQueue queue;

    auto requester1 = [](int count, MsgQueue& q) {
        for (int i = 0; i < count; ++i) {
            EXPECT_EQ(q.Request(Msg(i))->GetMsgId(), i + count);
        }
    };

    auto requester2 = [](int count, MsgQueue& q) {
        for (int i = 0; i < count; ++i) {
            EXPECT_EQ(q.Request(Msg(i + 2 * count))->GetMsgId(), i + 3 * count);
        }
    };

    auto responder = [](int count, MsgQueue& q) {
        for (int i = 0; i < 2 * count; ++i) {
            auto m = q.Get();
            q.RespondTo(m->GetUniqueId(), Msg(m->GetMsgId() + count));
        }
    };

    std::thread t1(requester1, N, std::ref(queue));
    std::thread t2(requester2, N, std::ref(queue));
    std::thread t3(responder, N, std::ref(queue));
    t1.join();
    t2.join();
    t3.join();
}

TEST(MsgQueue, InClass) {

struct M {
    int a;
    std::string s;

    M(int pa, std::string ps) : a(pa), s(ps) {}

    int GetA() const { return a; }
    std::string GetS() const { return s; } 
};

class X {
public:
    X() {};
    virtual ~X() = default;

    void Put(Msg&& m) {
        q_.Put(std::move(m));
    };

    void Run() {

        auto consumer = [this]() {
            auto m = q_.Get();
            auto& dm = dynamic_cast<DataMsg<M>&>(*m);
            EXPECT_EQ(dm.GetMsgId(), 42);
            EXPECT_EQ(dm.GetPayload().GetA(), 12);
            EXPECT_STREQ(dm.GetPayload().GetS().c_str(), "foo");
        };

        std::thread c(consumer);
        c.join();
    }

private:
    MsgQueue q_;
};

X x;

x.Put(DataMsg<M>(42, 12, "foo"));
x.Run();

M m(12, "foo");
x.Put(DataMsg<M>(42, m));
x.Run();

}
