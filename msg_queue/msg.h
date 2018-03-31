#ifndef COMPONENTS_MESSAGE_QUEUE_MSG_H_
#define COMPONENTS_MESSAGE_QUEUE_MSG_H_

#include <memory>
#include <utility>
#include <stdint.h>

// Type for Msg unique identifiers
using MsgUID = uint64_t;

/*
 * Msg represents a simple message that doesn't have any payload data.
 * Msg ID identifies the type of the message. Msg ID can be queried with getMsgId().
 */
class Msg {
public:
    Msg(int msg_id);

    virtual ~Msg() = default;
    Msg(const Msg&) = delete;
    Msg& operator=(const Msg&) = delete;

    // virtual move constructor
    virtual std::unique_ptr<Msg> move();

    int GetMsgId() const;

    MsgUID GetUniqueId() const;

protected:
    Msg(Msg&&) = default;
    Msg& operator=(Msg&&) = default;

private:
    int msg_id_;
    MsgUID unique_id_;

}; // Msg

/*
 * DataMsg<PayloadType> is a Msg with payload of type PayloadType.
 * Payload is constructed when DataMsg is created and the DataMsg instance owns the payload data.
 */
template <typename PayloadType>
class DataMsg : public Msg {
public:
    template <typename ... Args>
    DataMsg(int msg_id, Args&& ... args) 
      : Msg(msg_id),
        pl_(new PayloadType(std::forward<Args>(args) ...)) {
    }
    
    virtual ~DataMsg() = default;
    DataMsg(const DataMsg&) = delete;
    DataMsg& operator=(const DataMsg&) = delete;

    // virtual move constructor
    virtual std::unique_ptr<Msg> move() override {
        return std::unique_ptr<Msg>(new DataMsg<PayloadType>(std::move(*this)));
    }

    PayloadType& GetPayload() const {
        return *pl_;
    }

protected:
    DataMsg(DataMsg&&) = default;
    DataMsg& operator=(DataMsg&&) = default;

private:
    std::unique_ptr<PayloadType> pl_;

}; // DataMsg

#endif // COMPONENTS_MESSAGE_QUEUE_MSG_H_
