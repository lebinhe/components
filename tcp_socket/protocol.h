#ifndef COMPONENTS_TCP_SOCKET_PROTOCOL_H_
#define COMPONENTS_TCP_SOCKET_PROTOCOL_H_

#include <string>

namespace Socket
{

class DataSocket;

class Protocol {
protected:
    DataSocket& socket_;

public:
    Protocol(DataSocket& socket);
    virtual ~Protocol();

    virtual void SendMessage(std::string const& url, 
                             std::string const& message) = 0;
    virtual void RecvMessage(std::string& message)       = 0;
};

}

#endif // COMPONENTS_TCP_SOCKET_PROTOCOL_H_

