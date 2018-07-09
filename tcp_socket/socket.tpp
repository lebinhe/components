#include "components/tcp_socket/utility.h"

#include <stdexcept>

#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>

namespace Socket
{

template<typename F>
std::size_t DataSocket::GetMessageData(char* buffer, 
                                       std::size_t size,
                                       F scan_for_end) {
    if (GetSocketId() == 0) {
        throw std::logic_error(
            BuildErrorMessage(
                "DataSocket::",
                __func__, 
                ": accept called on a bad socket object (object was moved)"));
    }

    std::size_t data_read  = 0;
    while(data_read < size) {
        // The inner loop handles interactions with the socket.
        std::size_t get = read(GetSocketId(), 
                               buffer + data_read,
                               size - data_read);
        if (get == static_cast<std::size_t>(-1)) {
            switch(errno) {
                case EBADF:
                case EFAULT:
                case EINVAL:
                case ENXIO:
                {
                    // Fatal error. Programming bug
                    throw std::domain_error(
                        BuildErrorMessage(
                            "DataSocket::",
                            __func__,
                            ": read: critical error: ",
                            strerror(errno)));
                }
                case EIO:
                case ENOBUFS:
                case ENOMEM:
                {
                   // Resource acquisition failure or device error
                    throw std::runtime_error(
                        BuildErrorMessage(
                            "DataSocket::",
                            __func__, 
                            ": read: resource failure: ",
                            strerror(errno)));
                }
                case EINTR:
                    // TODO: Check for user interrupt flags.
                    //       Beyond the scope of this project
                    //       so continue normal operations.
                case ETIMEDOUT:
                case EAGAIN:
                {
                    // Temporary error.
                    // Simply retry the read.
                    continue;
                }
                case ECONNRESET:
                case ENOTCONN:
                {
                    // Connection broken.
                    // Return the data we have available and exit
                    // as if the connection was closed correctly.
                    get = 0;
                    break;
                }
                default:
                {
                    throw std::runtime_error(
                        BuildErrorMessage(
                            "DataSocket::",
                            __func__,
                            ": read: returned -1: ",
                            strerror(errno)));
                }
            }
        }
        if (get == 0) {
            break;
        }
        data_read += get;
        if (scan_for_end(data_read)) {
            break;
        }
    }

    return data_read;
}

}

