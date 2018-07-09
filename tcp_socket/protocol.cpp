#include "components/tcp_socket/protocol.h"

namespace Socket 
{

Protocol::Protocol(DataSocket& socket)
  : socket_(socket) {
}

Protocol::~Protocol() {
}

}
