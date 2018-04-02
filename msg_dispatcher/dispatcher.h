#ifndef COMPONENTS_MSG_DISPATHCER_DISPATCHER_H_
#define COMPONENTS_MSG_DISPATHCER_DISPATCHER_H_

#include "components/msg_queue/msg.h"

#include <cstdint>
#include <functional>
#include <map>
#include <memory>
#include <string>
#include <vector>

class Actor;

class Dispatcher {
public:
    void Register(std::shared_ptr<Actor> actor);

    void Post(const std::string& actor_name, Msg&& msg) const;

private:
    std::map<std::string, std::shared_ptr<Actor>> actors_;

}; 

#endif // COMPONENTS_MSG_DISPATHCER_DISPATCHER_H_
