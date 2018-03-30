#ifndef COMPONENTS_TIMING_WHEEL_TIMER_MANAGER_H_
#define COMPONENTS_TIMING_WHEEL_TIMER_MANAGER_H_

#include <stdint.h>

class ThreadTimer;

#define GRANULARITY 1 // 1ms
#define WHEEL_BITS1 8
#define WHEEL_BITS2 6
#define WHEEL_SIZE1 (1 << WHEEL_BITS1) // 256
#define WHEEL_SIZE2 (1 << WHEEL_BITS2) // 64
#define WHEEL_MASK1 (WHEEL_SIZE1 - 1)
#define WHEEL_MASK2 (WHEEL_SIZE2 - 1)
#define WHEEL_NUM 5

struct NodeLink {
    NodeLink *prev;
    NodeLink *next;

    NodeLink() { prev = next = this; }
};

struct TimerNode {
    NodeLink link;
    uint64_t dead_time;
    ThreadTimer *timer;

    TimerNode(ThreadTimer *t, uint64_t dt)
      : dead_time(dt), timer(t) {}
};

struct Wheel {
    NodeLink *spokes;
    uint32_t size;
    uint32_t spoke_index;

    Wheel(uint32_t n) : size(n), spoke_index(0) {
        spokes = new NodeLink[n];
    }

    ~Wheel() {
        if (spokes) {
            for (uint32_t j = 0; j < size; ++j) {
                NodeLink *link = (spokes + j)->next;
                while (link != spokes + j) {
                    TimerNode *node = (TimerNode*)link;
                    link = node->link.next;
                    delete node;
                }
            }
            delete []spokes;
            spokes = NULL;
        }
    }
};

class TimerManager {
public:
    TimerManager();
    virtual ~TimerManager();

    void DetectTimerList();
    TimerNode* AddTimer(uint32_t milseconds, ThreadTimer *timer);
    void RemoveTimer(TimerNode *node);
    virtual void Dump();

private:
    uint32_t Cascade(uint32_t wheel_index);
    void AddTimerNode(uint32_t milseconds, TimerNode *node);
    void AddToReadyNode(TimerNode *node);
    void DoTimeOutCallback();

private:
    Wheel *wheels_[WHEEL_NUM];
    uint64_t check_time_;
    NodeLink ready_nodes_;

}; // TimerManager

#endif // COMPONENTS_TIMING_WHEEL_TIMER_MANAGER_H_ 
