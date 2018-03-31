#ifndef COMPONENTS_MESSAGE_QUEUE_MSG_QUEUE_H_
#define COMPONENTS_MESSAGE_QUEUE_MSG_QUEUE_H_

#include "components/msg_queue/msg.h"
#include <memory>

// Msg ID for timeout message 
const int MSG_TIMEOUT = -1;

/*
 * Queue is a thread-safe message queue.
 * It supports one-way messaging and request-response pattern.
 */
class MsgQueue{
public:
    MsgQueue();
    
    ~MsgQueue();

    void Put(Msg&& msg);

    // Blocks until at least one message is available in the queue, 
    // or until timeout happens, 0 = wait indefinitely.
    std::unique_ptr<Msg> Get(int timeout_millis = 0);

    // Call will block until response is given with respondTo().
    std::unique_ptr<Msg> Request(Msg&& msg);

    // Respond to a request previously made with request().
    void RespondTo(MsgUID req_uid, Msg&& response_msg);

private:
    class Impl;
    std::unique_ptr<Impl> impl_;

}; // MsgQueue


#endif // COMPONENTS_MESSAGE_QUEUE_MSG_QUEUE_H_
