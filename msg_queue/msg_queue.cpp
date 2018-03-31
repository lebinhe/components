#include "components/message_queue/msg_queue.h"

#include <chrono>
#include <condition_variable>
#include <map>
#include <mutex>
#include <queue>
#include <utility>

class MsgQueue::Impl {
public:
    Impl()
      : queue_(), queue_mutex_(), queue_cond_(), response_map_(), response_map_mutex_() {
    }

    void Put(Msg&& msg) {
        {
            std::lock_guard<std::mutex> lock(queue_mutex_);
            queue_.push(msg.move());
        }

        queue_cond_.notify_one();
    }

    std::unique_ptr<Msg> Get(int timeout_millis) {
        std::unique_lock<std::mutex> lock(queue_mutex_);

        if (timeout_millis <= 0) {
            queue_cond_.wait(lock, [this]{return !queue_.empty();});
        } else {
            auto timeout_occured = !queue_cond_.wait_for(
                lock, 
                std::chrono::milliseconds(timeout_millis), 
                [this]{return !queue_.empty();});

            if (timeout_occured) {
                queue_.emplace(new Msg(MSG_TIMEOUT));
            }
        }

        auto msg = queue_.front()->move();
        queue_.pop();
        return msg;
    }

    std::unique_ptr<Msg> Request(Msg&& msg) {
        std::unique_lock<std::mutex> lock(response_map_mutex_);
        auto it = response_map_.emplace(
            std::make_pair(msg.GetUniqueId(), std::unique_ptr<MsgQueue>(new MsgQueue))).first;
        lock.unlock();

        Put(std::move(msg));
        auto response = it->second->Get(); // Block until response is put to the response MsgQueue

        lock.lock();
        response_map_.erase(it); // Delete the response MsgQueue
        lock.unlock();

        return response;
    }

    void RespondTo(MsgUID req_uid, Msg&& response_msg) {
        std::lock_guard<std::mutex> lock(response_map_mutex_);
        if (response_map_.count(req_uid) > 0) {
            response_map_[req_uid]->Put(std::move(response_msg));
        }
    }

private:
    // queue for msgs
    std::queue<std::unique_ptr<Msg>> queue_;

    // mutex to protect access to the queue
    std::mutex queue_mutex_;

    // condition variable to wait for when getting msg from the queue
    std::condition_variable queue_cond_;

    // map to keep track of which response handler queues are associated with request msg
    std::map<MsgUID, std::unique_ptr<MsgQueue>> response_map_;

    // mutex to protect access to response map
    std::mutex response_map_mutex_;

}; // MsgQueue::Impl


MsgQueue::MsgQueue()
  : impl_(new Impl) {
}

MsgQueue::~MsgQueue() {
}

void MsgQueue::Put(Msg&& msg) {
    impl_->Put(std::move(msg));
}

std::unique_ptr<Msg> MsgQueue::Get(int timeout_millis) {
    return impl_->Get(timeout_millis);
}

std::unique_ptr<Msg> MsgQueue::Request(Msg&& msg) {
    return impl_->Request(std::move(msg));
}

void MsgQueue::RespondTo(MsgUID req_uid, Msg&& response_msg) {
    impl_->RespondTo(req_uid, std::move(response_msg));
}
