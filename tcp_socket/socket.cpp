#include "components/tcp_socket/socket.h"

#include "components/tcp_socket/utility.h"

#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

#include <iostream>
#include <sstream>
#include <stdexcept>

namespace Socket
{

BaseSocket::BaseSocket(int socket_id)
  : socket_id_(socket_id) {
    if (socket_id_ == -1) {
        throw std::runtime_error(BuildErrorMessage("BaseSocket::",
                                                   __func__,
                                                   ": bad socket: ",
                                                   strerror(errno)));
    }
}

BaseSocket::~BaseSocket() {
    if (socket_id_ == invalid_socket_id_) {
        // This object has been closed or moved.
        // So we don't need to call close.
        return;
    }

    try {
        Close();
    } catch(...) {
        // We should log this 
        // TODO: LOGGING CODE HERE

        // If the user really want to catch close errors
        // they should call close() manually and handle
        // any generated exceptions. By using the
        // destructor they are indicating that failures is
        // an OK condition.
    }
}

void BaseSocket::Close() {
    if (socket_id_ == invalid_socket_id_) {
        throw std::logic_error(
                BuildErrorMessage(
                    "DataSocket::",
                    __func__,
                    ": accept called on a bad socket object (object was moved)"));
    }
    while(true) {
        int state = ::close(socket_id_);
        if (state == invalid_socket_id_) {
            break;
        }
        switch(errno) {
            case EBADF: 
                throw std::domain_error(
                        BuildErrorMessage("BaseSocket::",
                                          __func__,
                                          ": close: EBADF: ",
                                          socket_id_,
                                          " ",
                                          strerror(errno)));
            case EIO:   
                throw std::runtime_error(BuildErrorMessage("BaseSocket::", 
                                                           __func__,
                                                           ": close: EIO:  ",
                                                           socket_id_,
                                                           " ",
                                                           strerror(errno)));
            case EINTR: {
                        // TODO: Check for user interrupt flags.
                        //       Beyond the scope of this project
                        //       so continue normal operations.
                break;
            }
            default:    
                throw std::runtime_error(BuildErrorMessage("BaseSocket::",
                                                            __func__,
                                                            ": close: ???:  ",
                                                            socket_id_,
                                                            " ",
                                                            strerror(errno)));
        }
    }
    socket_id_ = invalid_socket_id_;
}

void BaseSocket::swap(BaseSocket& other) noexcept {
    std::swap(socket_id_, other.socket_id_);
}

BaseSocket::BaseSocket(BaseSocket&& move) noexcept
  : socket_id_(invalid_socket_id_) {
    move.swap(*this);
}

BaseSocket& BaseSocket::operator=(BaseSocket&& move) noexcept {
    move.swap(*this);
    return *this;
}

ConnectSocket::ConnectSocket(std::string const& host, int port)
  : DataSocket(::socket(PF_INET, SOCK_STREAM, 0)) {
    struct sockaddr_in server_addr {};
    server_addr.sin_family       = AF_INET;
    server_addr.sin_port         = htons(port);
    server_addr.sin_addr.s_addr  = inet_addr(host.c_str());

    if (::connect(GetSocketId(), 
                  (struct sockaddr*)&server_addr,
                  sizeof(server_addr)) != 0) {
        Close();
        throw std::runtime_error(BuildErrorMessage("ConnectSocket::",
                                                   __func__,
                                                   ": connect: ",
                                                   strerror(errno)));
    }
}

ServerSocket::ServerSocket(int port)
  : BaseSocket(::socket(PF_INET, SOCK_STREAM, 0)) {
    struct sockaddr_in server_addr;
    bzero((char*)&server_addr, sizeof(server_addr));
    server_addr.sin_family       = AF_INET;
    server_addr.sin_port         = htons(port);
    server_addr.sin_addr.s_addr  = INADDR_ANY;

    if (::bind(GetSocketId(), (struct sockaddr *) &server_addr, 
               sizeof(server_addr)) != 0) {
        Close();
        throw std::runtime_error(BuildErrorMessage(
                                     "ServerSocket::",
                                     __func__,
                                     ": bind: ",
                                     strerror(errno)));
    }

    if (::listen(GetSocketId(), max_connection_backlog_) != 0) {
        Close();
        throw std::runtime_error(BuildErrorMessage(
                                     "ServerSocket::",
                                     __func__,
                                     ": listen: ",
                                     strerror(errno)));
    }
}

DataSocket ServerSocket::Accept() {
    if (GetSocketId() == invalid_socket_id_) {
        throw std::logic_error(
                BuildErrorMessage(
                "ServerSocket::",
                __func__,
                ": accept called on a bad socket object (object was moved)"));
    }

    struct  sockaddr_storage    server_storage;
    socklen_t                   addr_size   = sizeof server_storage;
    int new_socket = ::accept(GetSocketId(), (struct sockaddr*)&server_storage, &addr_size);
    if (new_socket == -1) {
        throw std::runtime_error(BuildErrorMessage(
                                     "ServerSocket:",
                                     __func__,
                                     ": accept: ",
                                     strerror(errno)));
    }
    return DataSocket(new_socket);
}

void DataSocket::PutMessageData(char const* buffer, std::size_t size) {
    std::size_t data_written = 0;

    while(data_written < size) {
        std::size_t put = write(GetSocketId(), 
                                buffer + data_written,
                                size - data_written);
        if (put == static_cast<std::size_t>(-1)) {
            switch(errno) {
                case EINVAL:
                case EBADF:
                case ECONNRESET:
                case ENXIO:
                case EPIPE:
                {
                    // Fatal error. Programming bug
                    throw std::domain_error(
                            BuildErrorMessage("DataSocket::",
                                              __func__,
                                              ": write: critical error: ",
                                              strerror(errno)));
                }
                case EDQUOT:
                case EFBIG:
                case EIO:
                case ENETDOWN:
                case ENETUNREACH:
                case ENOSPC:
                {
                    // Resource acquisition failure or device error
                    throw std::runtime_error(
                            BuildErrorMessage("DataSocket::",
                                               __func__,
                                               ": write: resource failure: ",
                                               strerror(errno)));
                }
                case EINTR:
                        // TODO: Check for user interrupt flags.
                        //       Beyond the scope of this project
                        //       so continue normal operations.
                case EAGAIN:
                {
                    // Temporary error.
                    // Simply retry the read.
                    continue;
                }
                default:
                {
                    throw std::runtime_error(
                            BuildErrorMessage("DataSocket::",
                                              __func__,
                                              ": write: returned -1: ",
                                              strerror(errno)));
                }
            }
        }
        data_written += put;
    }
    return;
}

void DataSocket::PutMessageClose() {
    if (::shutdown(GetSocketId(), SHUT_WR) != 0) {
        throw std::domain_error(BuildErrorMessage("HTTPProtocol::",
                                                  __func__,
                                                  ": shutdown: critical error: ",
                                                  strerror(errno)));
    }
}

}
