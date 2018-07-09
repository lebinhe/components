#include "components/tcp_socket/protocol_simple.h"
#include "components/tcp_socket/socket.h"

#include <cstdlib>
#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: client <host> <Message>\n";
        std::exit(1);
    }

    Socket::ConnectSocket connect(argv[1], 12345);
    Socket::DataSocket& ds = connect;
    Socket::ProtocolSimple   simple_connect(ds);
    simple_connect.SendMessage("", argv[2]);

    std::string message;
    simple_connect.RecvMessage(message);
    std::cout << message << "\n";
}
