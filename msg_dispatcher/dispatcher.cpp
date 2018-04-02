#include "components/msg_dispatcher/dispatcher.h"
#include "components/msg_dispatcher/actor.h"

#include <algorithm>

void Dispatcher::Register(std::shared_ptr<Actor> actor) {
    if (actor.get()) {
        actors_[actor->Name()] = actor;
    }
}

void Dispatcher::Post(const std::string& actor_name, Msg&& msg) const {
    auto it = actors_.find(actor_name);
    if (it != actors_.end()) {
        it->second->Put(std::move(msg));
    }
}
