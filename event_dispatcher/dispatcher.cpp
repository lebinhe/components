#include "components/event_dispatcher/dispatcher.h"
#include "components/event_dispatcher/subscriber.h"

#include <algorithm>

Subscriber Dispatcher::Subscribe(const Event::Descriptor& descriptor, SlotType&& slot) {
    SlotHandle handle = {next_id_++, slot};
    subscribers_[descriptor].emplace_back(handle);
    return Subscriber(this, handle.id);
}

void Dispatcher::UnSubscribe(const Subscriber& subscriber) {
    for (auto&& pair : subscribers_) {
        auto&& handles = pair.second;

        handles.erase(std::remove_if(handles.begin(), handles.end(), 
                                     [&] (SlotHandle& handle){
                                        return handle.id == subscriber.Id();
                                     }), 
                      handles.end());
    }
}

void Dispatcher::Post(const Event& event) const {
    auto type = event.Type();

    if (subscribers_.find(type) == subscribers_.end()) {
        return;
    }

    auto&& subscribers = subscribers_.at(type);

    for (auto&& subscriber : subscribers) {
        subscriber.slot(event);
    }
}
