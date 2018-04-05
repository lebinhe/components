#include "components/event_dispatcher/dispatcher.h"
#include "components/event_dispatcher/subscriber.h"

void Subscriber::UnSubscribe() {
    if(dispatcher_) {
        dispatcher_->UnSubscribe(*this);
    }
}
