#include "components/timing_wheel/thread_timer.h"
#include "components/timing_wheel/timer_manager.h"

ThreadTimer::~ThreadTimer() {
}

bool ThreadTimer::Start() {
    Stop();
    status_ = TIME_OUT;
    timer_node_ = powner_->AddTimer(interval_, this);
    return timer_node_ != NULL; 
}

void ThreadTimer::Stop() {
    status_ = USER_CANCLE;
    if (timer_node_) {
        powner_->RemoveTimer(timer_node_);
        timer_node_ = NULL;
    }
}

void ThreadTimer::Callback() {
    timer_node_ = NULL;
    if (type_ == TIMER_REPEAT) {
        Start();
    }

    callback_(); //must at the last line; timer may be deleted in callback_();
}
