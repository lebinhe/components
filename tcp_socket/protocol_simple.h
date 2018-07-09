#ifndef COMPONENTS_TCP_SOCKET_PROTOCOL_SIMPLE_H_
#define COMPONENTS_TCP_SOCKET_PROTOCOL_SIMPLE_H_

#include "components/tcp_socket/protocol.h"

#include <string>

namespace Socket
{

class ProtocolSimple : public Protocol {
public:
    ProtocolSimple(DataSocket& socket) : Protocol(socket) { }

    void SendMessage(std::string const& url, 
                     std::string const& message) override;

    void RecvMessage(std::string& message)       override;
};

}

#endif // COMPONENTS_TCP_SOCKET_PROTOCOL_SIMPLE_H_
