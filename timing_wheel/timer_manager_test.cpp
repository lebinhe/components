#include "thirdparty/glog/logging.h"
#include "thirdparty/gtest/gtest.h"
#include "components/timing_wheel/thread_timer.h"
#include "components/timing_wheel/timer_manager.h"

#include <functional>


void callback1() { LOG(ERROR) << __FUNCTION__; }
void callback2() { LOG(ERROR) << __FUNCTION__; }
void callback3() { LOG(ERROR) << __FUNCTION__; }
void callback4(void *p) { LOG(ERROR) << __FUNCTION__ << "---" << p; }


TEST(TimerManager, Dump) {
    uint32_t i1= 1, i2 = 20, i3 = 300, i4 = 255, i5 = (1 << 14) - 1, i6 = (1 << 14), i7 = (1 << 20), i8 = (1 << 26), i9 = (1 << 30) - 1;
    TimerManager tm;
    ThreadTimer timer1(&tm, i1, (std::function<void ()>)(&callback1), ThreadTimer::TIMER_REPEAT);
    ThreadTimer timer2(&tm, i2, (std::function<void ()>)(&callback2));
    ThreadTimer timer3(&tm, i3, (std::function<void ()>)(&callback3), ThreadTimer::TIMER_REPEAT);
    ThreadTimer timer4(&tm, i4, (std::function<void ()>)(std::bind(&callback4, (void*)0X001)), ThreadTimer::TIMER_REPEAT);
    ThreadTimer timer5(&tm, i5, (std::function<void ()>)(&callback2));
    ThreadTimer timer6(&tm, i6, (std::function<void ()>)(&callback2));
    ThreadTimer timer7(&tm, i7, (std::function<void ()>)(&callback2));
    ThreadTimer timer8(&tm, i8, (std::function<void ()>)(&callback2));
    ThreadTimer timer9(&tm, i9, (std::function<void ()>)(&callback2));
    timer1.Start();
    LOG(ERROR) << "start timer1, interval = " << i1;
    timer2.Start();
    LOG(ERROR) << "start timer2, interval = " << i2;
    timer3.Start();
    LOG(ERROR) << "start timer3, interval = " << i3;
    timer4.Start();
    LOG(ERROR) << "start timer4, interval = " << i4;
    timer5.Start();
    LOG(ERROR) << "start timer5, interval = " << i5;
    timer6.Start();
    LOG(ERROR) << "start timer6, interval = " << i6;
    timer7.Start();
    LOG(ERROR) << "start timer7, interval = " << i7;
    timer8.Start();
    LOG(ERROR) << "start timer8, interval = " << i8;
    timer9.Start();
    LOG(ERROR) << "start timer9, interval = " << i9;
    tm.Dump();

    timer2.Stop();
    LOG(ERROR) << "stop timer2, interval = " << i2;
    tm.Dump();

    timer1.Stop();
    LOG(ERROR) << "stop timer1, interval = " << i1;
    timer3.Stop();
    LOG(ERROR) << "stop timer3, interval = " << i3;
    tm.Dump();

    timer1.Start();
    LOG(ERROR) << "start timer1, interval = " << i1;
    timer3.Start();
    LOG(ERROR) << "start timer3, interval = " << i3;
    timer2.Start();
    LOG(ERROR) << "start timer2, interval = " << i2;
    tm.Dump();
    timer1.Start();
    LOG(ERROR) << "start timer1, interval = " << i1;
    tm.Dump();
}

TEST(TimerManager, DetectTimerList) {
    uint32_t i1= 1000, i2 = 10000;
    TimerManager tm;
    ThreadTimer timer1(&tm, i1, (std::function<void ()>)(&callback1), ThreadTimer::TIMER_REPEAT);
    timer1.Start();
    ThreadTimer timer2(&tm, i2, (std::function<void ()>)(&callback2), ThreadTimer::TIMER_ONCE);
    timer2.Start();
    tm.Dump();

    for (int i = 0; i < 100000; ++i) {
        tm.DetectTimerList();
        usleep(1000);
    }
}

TEST(TimerManager, Sleep) {
    sleep(5);
}
