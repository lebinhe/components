#ifndef COMPONENTS_EVENT_DISPATHCER_DISPATCHER_H_
#define COMPONENTS_EVENT_DISPATHCER_DISPATCHER_H_

#include "components/event_dispatcher/event.h"

#include <functional>
#include <map>
#include <vector>
#include <stdint.h>

class Subscriber;

class Dispatcher {
public:
    using SlotType = std::function<void(const Event&)>;

    Subscriber Subscribe(const Event::Descriptor& descriptor, SlotType&& slot);

    void UnSubscribe(const Subscriber& subscriber);

    void Post(const Event& event) const;

private:
    uint64_t next_id_ = 0;

    struct SlotHandle {
        uint64_t id;
        SlotType slot;
    };

    std::map<Event::Descriptor, std::vector<SlotHandle>> subscribers_;

}; // Dispatcher

#endif // COMPONENTS_EVENT_DISPATHCER_DISPATCHER_H_
