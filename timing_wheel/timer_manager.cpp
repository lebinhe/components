#include "components/timing_wheel/thread_timer.h"
#include "components/timing_wheel/timer_manager.h"

#include <sys/time.h>

static uint64_t GetCurrentMillisec() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (uint64_t)tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

TimerManager::TimerManager() {
    check_time_ = GetCurrentMillisec();
    wheels_[0] = new Wheel(WHEEL_SIZE1);
    for (int i = 1; i < WHEEL_NUM; ++i) {
        wheels_[i] = new Wheel(WHEEL_SIZE2);
    }
}

TimerManager::~TimerManager() {
    for (int i = 0; i < WHEEL_NUM; ++i) {
        if (wheels_[i]) { 
            delete wheels_[i];
            wheels_[i] = NULL;
        }
    }
}

void TimerManager::DetectTimerList() {
    uint64_t now = GetCurrentMillisec();
    uint32_t loopnum = now > check_time_ ? (now - check_time_) / GRANULARITY : 0;

    Wheel *wheel = wheels_[0];
    for (uint32_t i = 0; i < loopnum; ++i) {
        NodeLink *spoke = wheel->spokes + wheel->spoke_index;
        NodeLink *link = spoke->next;
        while (link != spoke) {
            TimerNode *node = (TimerNode*)link;
            link->prev->next = link->next;
            link->next->prev = link->prev;
            link = node->link.next;
            AddToReadyNode(node);
        }
        if (++(wheel->spoke_index) >= wheel->size) {
            wheel->spoke_index = 0;
            Cascade(1);
        }
        check_time_ += GRANULARITY;
    }
    DoTimeOutCallback();
}

void TimerManager::AddToReadyNode(TimerNode *node) {
    NodeLink *node_link = &(node->link);
    node_link->prev = ready_nodes_.prev;
    ready_nodes_.prev->next = node_link;
    node_link->next = &ready_nodes_;
    ready_nodes_.prev = node_link;
}

void TimerManager::DoTimeOutCallback() {
    NodeLink *link = ready_nodes_.next;
    while (link != &ready_nodes_) {
        TimerNode *node = (TimerNode*)link;
        node->timer->Callback();
        link = node->link.next;
        delete node;
    }
    ready_nodes_.next = ready_nodes_.prev = &ready_nodes_;
}

uint32_t TimerManager::Cascade(uint32_t wheel_index) {
    if (wheel_index < 1 || wheel_index >= WHEEL_NUM) {
        return 0;
    }

    Wheel *wheel = wheels_[wheel_index];
    int casnum = 0;
    uint64_t now = GetCurrentMillisec();
    NodeLink  *spoke = wheel->spokes + (wheel->spoke_index++);
    NodeLink *link = spoke->next;
    spoke->next = spoke->prev = spoke;

    while (link != spoke) {
        TimerNode *node = (TimerNode*)link;
        link = node->link.next;
        if (node->dead_time <= now) {
            AddToReadyNode(node);
        } else {
            uint32_t milseconds = node->dead_time - now;
            AddTimerNode(milseconds, node);
            ++casnum;
        }
    }

    if (wheel->spoke_index >= wheel->size) {
        wheel->spoke_index = 0;
        casnum += Cascade(++wheel_index);
    }
    return casnum;
}

TimerNode* TimerManager::AddTimer(uint32_t milseconds, ThreadTimer *timer) {
    uint64_t dead_time = GetCurrentMillisec() + milseconds;
    TimerNode *node = new TimerNode(timer, dead_time);
    AddTimerNode(milseconds, node);
    return node;
}

void TimerManager::AddTimerNode(uint32_t milseconds, TimerNode *node) {
    NodeLink *spoke = NULL;
    uint32_t interval = milseconds / GRANULARITY;
    uint32_t threshold1 = WHEEL_SIZE1;
    uint32_t threshold2 = 1 << (WHEEL_BITS1 + WHEEL_BITS2);
    uint32_t threshold3 = 1 << (WHEEL_BITS1 + 2 * WHEEL_BITS2);
    uint32_t threshold4 = 1 << (WHEEL_BITS1 + 3 * WHEEL_BITS2);

    if (interval < threshold1) {
        uint32_t index = (interval + wheels_[0]->spoke_index) & WHEEL_MASK1;
        spoke = wheels_[0]->spokes + index;
    } else if (interval < threshold2) {
        uint32_t index = ((interval - threshold1 + wheels_[1]->spoke_index * threshold1) >> WHEEL_BITS1) & WHEEL_MASK2;
        spoke = wheels_[1]->spokes + index;
    } else if (interval < threshold3) {
        uint32_t index = ((interval - threshold2 + wheels_[2]->spoke_index * threshold2) >> (WHEEL_BITS1 + WHEEL_BITS2)) & WHEEL_MASK2;
        spoke = wheels_[2]->spokes + index;
    } else if (interval < threshold4) {
        uint32_t index = ((interval - threshold3 + wheels_[3]->spoke_index * threshold3) >> (WHEEL_BITS1 + 2 * WHEEL_BITS2)) & WHEEL_MASK2;
        spoke = wheels_[3]->spokes + index;
    } else {
        uint32_t index = ((interval - threshold4 + wheels_[4]->spoke_index * threshold4) >> (WHEEL_BITS1 + 3 * WHEEL_BITS2)) & WHEEL_MASK2;
        spoke = wheels_[4]->spokes + index;
    }

    NodeLink *node_link = &(node->link);
    node_link->prev = spoke->prev;
    spoke->prev->next = node_link;
    node_link->next = spoke;
    spoke->prev = node_link;
}

void TimerManager::RemoveTimer(TimerNode *node) {
    NodeLink *node_link = &(node->link);
    if (node_link->prev) {
        node_link->prev->next = node_link->next;
    }
    if (node_link->next) {
        node_link->next->prev = node_link->prev;
    }
    node_link->prev = node_link->next = NULL;

    delete node;
}

void TimerManager::Dump() {
    for (int i = 0; i < WHEEL_NUM; ++i) {
        Wheel *wheel = wheels_[i];
        printf("    wheel[%d].size[%u], spoke_index[%u]:\n", 
            i, wheel->size, wheel->spoke_index);
        for (uint32_t j = 0; j < wheel->size; ++j) {
            NodeLink *spoke = wheel->spokes + j;
            NodeLink *link = spoke->next;
            if (link != spoke) {
                printf("       spoke index[%d], addr[%p], next[%p], prev[%p]\n",
                    j, spoke, spoke->next, spoke->prev);
            }
            while (link != spoke) {
                TimerNode *node = (TimerNode*)link;
                printf("          node[%p], next[%p], prev[%p] interval[%u], dead_time[%lu]\n",
                    link, link->next, link->prev, node->timer->GetInterval(), node->dead_time);
                link = node->link.next;
            }
        }
    }
}

