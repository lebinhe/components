#ifndef COMPONENTS_TCP_SOCKET_SOCKET_H_
#define COMPONENTS_TCP_SOCKET_SOCKET_H_

#include <sstream>
#include <string>
#include <vector>

namespace Socket
{

// An RAII base class for handling sockets.
// Socket is movable but not copyable.
class BaseSocket {
private:
    int socket_id_;

protected:
    static constexpr int invalid_socket_id_ = -1;

    // Designed to be a base class not used used directly.
    BaseSocket(int socket_id);
    int GetSocketId() const { return socket_id_; }

public:
    virtual ~BaseSocket();

    // Moveable but not Copyable
    BaseSocket(BaseSocket&& move)               noexcept;
    BaseSocket& operator=(BaseSocket&& move)    noexcept;
    void swap(BaseSocket& other)                noexcept;
    BaseSocket(BaseSocket const&)               = delete;
    BaseSocket& operator=(BaseSocket const&)    = delete;

    // User can manually call close
    void Close();
};

// A class that can read/write to a socket
class DataSocket : public BaseSocket {
public:
    DataSocket(int socket_id) : BaseSocket(socket_id) {
    }

    template<typename F>
    std::size_t GetMessageData(char* buffer, 
                               std::size_t size,
                               F scan_for_end = [](std::size_t){ 
                                   return false; 
                               });
    void PutMessageData(char const* buffer, std::size_t size);
    void PutMessageClose();
};

// A class the conects to a remote machine
// Allows read/write accesses to the remote machine
class ConnectSocket : public DataSocket {
public:
    ConnectSocket(std::string const& host, int port);
};

// A server socket that listens on a port for a connection
class ServerSocket : public BaseSocket {
private:
    static constexpr int max_connection_backlog_ = 5;

public:
    ServerSocket(int port);

    // An accepts waits for a connection and returns a socket
    // object that can be used by the client for communication
    DataSocket Accept();
};

}

#include "components/tcp_socket/socket.tpp"

#endif // COMPONENTS_TCP_SOCKET_SOCKET_H_
