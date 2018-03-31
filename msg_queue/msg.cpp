#include "components/msg_queue/msg.h"
#include <atomic>

namespace {

MsgUID GenerateUniqueId() {
    static std::atomic<MsgUID> i(0);
    return ++i;
}

}

Msg::Msg(int msg_id)
  : msg_id_(msg_id), unique_id_(GenerateUniqueId()){
}

std::unique_ptr<Msg> Msg::move() {
    return std::unique_ptr<Msg>(new Msg(std::move(*this)));
}

int Msg::GetMsgId() const {
    return msg_id_;
}

MsgUID Msg::GetUniqueId() const {
    return unique_id_;
}
