#ifndef COMPONENTS_EVENT_DISPATCHER_EVENT_H_
#define COMPONENTS_EVENT_DISPATCHER_EVENT_H_

#include <string>

class Event {
public:
    virtual ~Event() {}

    using Descriptor = const char*;

    virtual Descriptor Type() const = 0;

}; // event

#endif // COMPONENTS_EVENT_DISPATCHER_EVENT_H_
