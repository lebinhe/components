#include "components/tcp_socket/protocol_simple.h"
#include "components/tcp_socket/socket.h"

#include <iostream>
#include <string>

int main() {
    Socket::ServerSocket server(12345);
    int finished = 0;
    while(!finished) {
        Socket::DataSocket accept  = server.Accept();
        Socket::ProtocolSimple accept_simple(accept);

        std::string message;
        accept_simple.RecvMessage(message);
        std::cout << message << "\n";

        accept_simple.SendMessage("", "OK");
    }
}
