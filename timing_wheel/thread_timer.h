#ifndef COMPONENTS_TIMING_WHEEL_THREAD_TIMER_H_
#define COMPONENTS_TIMING_WHEEL_THREAD_TIMER_H_


#include <functional>
#include <stdint.h>

class TimerManager;
struct TimerNode;

class ThreadTimer {
public:
    typedef enum { TIMER_ONCE = 0, TIMER_REPEAT } Type;
    typedef enum { WAITING = 0, TIME_OUT, USER_CANCLE } Status;

    template<typename Callback>
    ThreadTimer(TimerManager *m,
                uint32_t i = 0,
                Callback c = 0,
                Type t = TIMER_ONCE,
                void *data = NULL)
      : powner_(m), 
        interval_(i),
        callback_(c),
        type_(t),
        status_(WAITING),
        timer_node_(NULL), 
        data_(data) {}

    ~ThreadTimer();

    template<typename Callback>
    void Init(TimerManager *m, 
              uint32_t i = 0,
              Callback c = 0,
              Type t = TIMER_ONCE,
              void *data = NULL) {
        powner_ = m;
        interval_ = i;
        callback_ = c;
        type_ = t;
        data_ = data;
    }

    bool Start();
    void Stop();
    int GetStatus() { return status_; }
    void SetData(void *data) { data_ = data; }
    void *GetData() { return data_; }
    void SetInterval(uint32_t i) { interval_ = i; }
    uint32_t GetInterval() { return interval_; }

    void Callback();

private:
    TimerManager *powner_;
    uint32_t interval_;     // unit : ms
    std::function<void()> callback_;
    Type type_;
    Status status_;
    TimerNode *timer_node_;
    void *data_;

}; // ThreadTimer

#endif // COMPONENTS_TIMING_WHEEL_THREAD_TIMER_H_
