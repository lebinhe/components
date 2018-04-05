#ifndef COMPONENTS_EVENT_DISPATHCER_SUBSCRIBER_H_
#define COMPONENTS_EVENT_DISPATHCER_SUBSCRIBER_H_

#include "components/event_dispatcher/event.h"

#include <stdint.h>

class Dispatcher;

class Subscriber {
public:
    Subscriber() {}

    uint64_t Id() const { return id_; }

    void UnSubscribe();

private:
    Subscriber(Dispatcher* dispatcher, uint64_t id)
      : dispatcher_(dispatcher),
        id_(id) {
    }

    Dispatcher* dispatcher_ = nullptr;
    uint64_t id_ = 0;

    friend class Dispatcher;

}; // Subscriber


#endif // COMPONENTS_EVENT_DISPATHCER_SUBSCRIBER_H_
