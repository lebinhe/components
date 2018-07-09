#include "components/tcp_socket/protocol_simple.h"

#include "components/tcp_socket/socket.h"

#include <iostream>

namespace Socket 
{

void ProtocolSimple::SendMessage(std::string const& url, 
                                 std::string const& message) {
    socket_.PutMessageData(url.c_str(), url.size());
    socket_.PutMessageData(message.c_str(), message.size());
    socket_.PutMessageClose();
}

// Utility class
// Used by GetMessage() to open the string upto capacity size.
// Then on destruction resize to the actual size of the string.
class StringSizer {
    std::string& string_data_;
    std::size_t& current_size_;
    public:
        StringSizer(std::string& string_data, std::size_t& current_size)
          : string_data_(string_data), 
            current_size_(current_size) {
            if (string_data.capacity() == 0) {
                string_data.resize(4096);
            }
            string_data_.resize(string_data.capacity());
        }

        ~StringSizer() {
            string_data_.resize(current_size_);
        }

        void IncrementSize(std::size_t amount) {
            current_size_ += amount;
        }
};

void ProtocolSimple::RecvMessage(std::string& message) {
    std::size_t data_read = 0;
    message.clear();

    while(true) {
        // This outer loop handles resizing of the message 
        // when we run of space in the string.
        StringSizer        string_sizer(message, data_read);
        std::size_t const  data_max  = message.capacity() - 1;
        char*              buffer   = &message[0];

        std::size_t got = socket_.GetMessageData(buffer + data_read, 
                                                data_max - data_read,
                                                [](std::size_t){return false;});
        data_read += got;
        if (got == 0) {
            break;
        }

        // Resize the string buffer
        // So that next time around we can read more data.
        message.reserve(message.capacity() * 2);
    }
}

}
