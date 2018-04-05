#include "components/event_dispatcher/dispatcher.h"
#include "components/event_dispatcher/event.h"
#include "components/event_dispatcher/subscriber.h"
#include "thirdparty/glog/logging.h"
#include "thirdparty/gtest/gtest.h"

#include <functional>
#include <iostream>

class DemoEvent : public Event {
public:
    DemoEvent() {
        std::cout << __PRETTY_FUNCTION__ << std::endl;
    }

    virtual ~DemoEvent() {
        std::cout << __PRETTY_FUNCTION__ << std::endl;
    }

    static constexpr Descriptor descriptor = "DemoEvent";

    virtual Descriptor Type() const {
        return descriptor;
    }
};
constexpr DemoEvent::Descriptor DemoEvent::descriptor;


void FreeSubscriber(const Event& e) {
    if (e.Type() == DemoEvent::descriptor) {
        std::cout << __PRETTY_FUNCTION__ << ": DemoEvent" << std::endl;
    }
}


class ClassSubscriber {
public:
    void Handle(const Event& e) {
        if (e.Type() == DemoEvent::descriptor) {
            const DemoEvent& de = static_cast<const DemoEvent&>(e);
            std::cout << __PRETTY_FUNCTION__ << ": " << de.Type() << std::endl;
        }
    }
};


class DispatcherTest : public testing::Test {
public:
    static void SetUpTestCase() {
        class_subscriber = new ClassSubscriber;
        dispatcher = new Dispatcher;
    }

    static void TearDownTestCase() {
        delete class_subscriber;
        class_subscriber = NULL;

        delete dispatcher;
        dispatcher = NULL;
    }

    static ClassSubscriber* class_subscriber;
    static Dispatcher* dispatcher;
};

ClassSubscriber* DispatcherTest::class_subscriber = NULL;
Dispatcher* DispatcherTest::dispatcher = NULL;

TEST_F(DispatcherTest, Test) {
    auto s1 = DispatcherTest::dispatcher->Subscribe(DemoEvent::descriptor, FreeSubscriber);
    auto s2 = DispatcherTest::dispatcher->Subscribe(DemoEvent::descriptor, 
                                                    std::bind(&ClassSubscriber::Handle, 
                                                              *DispatcherTest::class_subscriber,
                                                              std::placeholders::_1));

    std::cout << "#\n"
              << "# Posting a demo event. This should trigger two subscribers\n"
              << "#\n";

    DispatcherTest::dispatcher->Post(DemoEvent());

    s1.UnSubscribe();

    std::cout << "#\n"
              << "# Posting a demo event. This should trigger one observer\n"
              << "#\n";

    DispatcherTest::dispatcher->Post(DemoEvent());

    s2.UnSubscribe();

    std::cout << "#\n"
              << "# Posting a demo event. This should trigger no observers\n"
              << "#\n";

    DispatcherTest::dispatcher->Post(DemoEvent());

    s1.UnSubscribe();
    s2.UnSubscribe();
}
