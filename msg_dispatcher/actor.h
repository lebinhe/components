#ifndef COMPONENTS_MSG_DISPATHCER_ACTOR_H_
#define COMPONENTS_MSG_DISPATHCER_ACTOR_H_

#include "components/msg_queue/msg.h"
#include "components/msg_queue/msg_queue.h"

#include <cstdint>
#include <string>

class Dispatcher;

class Actor {
public:
    Actor(const Dispatcher* dispatcher, const std::string& name)
      : dispatcher_(dispatcher),
        name_(name) {
    }

    virtual ~Actor() {};
    Actor (const Actor&) = delete;
    Actor& operator=(const Actor&) = delete;

    const std::string& Name() const { return name_; }

    void Put(Msg&& msg) { msg_queue_.Put(std::move(msg)); }

    virtual void Handle() = 0;

protected:
    const Dispatcher* dispatcher_ = nullptr;
    std::string name_;
    MsgQueue msg_queue_;
}; 

#endif // COMPONENTS_MSG_DISPATHCER_ACTOR_H_
