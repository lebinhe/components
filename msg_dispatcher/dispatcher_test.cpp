#include "components/msg_dispatcher/dispatcher.h"
#include "components/msg_dispatcher/actor.h"
#include "components/msg_queue/msg.h"
#include "components/msg_queue/msg_queue.h"
#include "thirdparty/glog/logging.h"
#include "thirdparty/gtest/gtest.h"

#include <functional>
#include <iostream>
#include <string>
#include <thread>

const int MSG_FOO = 1, MSG_BAR = 2;

class DemoMsg {
public:
    DemoMsg(const std::string& content) : content_(content) {
        std::cout << __PRETTY_FUNCTION__ << std::endl;
    }

    virtual ~DemoMsg() {
        std::cout << __PRETTY_FUNCTION__ << std::endl;
    }

    const std::string Content() const { return content_; }

private:
    std::string content_;
};

class DemoActor : public Actor {
public:
    DemoActor(const Dispatcher* dispatcher, const std::string& name) 
      : Actor(dispatcher, name) {
    }

    virtual ~DemoActor() {}

    virtual void Handle() override {
        std::thread t([this]() {
                    for (; !stop_; ) {
                        auto m = msg_queue_.Get(2000);
                        if (m->GetMsgId() == MSG_TIMEOUT) {
                            std::cout << "---timeout---" << std::endl;
                            stop_ = true;
                        } else {
                            auto& dm = dynamic_cast<DataMsg<DemoMsg>&>(*m);
                            if (dm.GetMsgId() == MSG_FOO) {
                                EXPECT_STREQ(dm.GetPayload().Content().c_str(), "foo");
                                this->dispatcher_->Post("DemoActor", DataMsg<DemoMsg>(MSG_BAR, "bar"));
                            } else if (dm.GetMsgId() == MSG_BAR) {
                                EXPECT_STREQ(dm.GetPayload().Content().c_str(), "bar");
                            } 
                            std::cout << "msg content: " << dm.GetPayload().Content() << std::endl;
                        }
                    }
                });
        t.join();
    }
 
private:
    bool stop_ = false;
};

class DispatcherTest : public testing::Test {
public:
    static void SetUpTestCase() {
        dispatcher = new Dispatcher;
    }

    static void TearDownTestCase() {
        delete dispatcher;
        dispatcher = NULL;
    }

    static Dispatcher* dispatcher;
};

Dispatcher* DispatcherTest::dispatcher = NULL;

TEST_F(DispatcherTest, Test) {
    std::shared_ptr<DemoActor> dh(new DemoActor(DispatcherTest::dispatcher, "DemoActor"));
    DispatcherTest::dispatcher->Register(dh);

    std::cout << "#\n"
              << "# Posting a demo msg.\n"
              << "#\n";

    DemoMsg dm("foo");
    DispatcherTest::dispatcher->Post("DemoActor", DataMsg<DemoMsg>(MSG_FOO, dm));
    dh->Handle();
}
