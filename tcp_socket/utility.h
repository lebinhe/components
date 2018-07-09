#ifndef COMPONENTS_TCP_SOCKET_UTILITY_H_
#define COMPONENTS_TCP_SOCKET_UTILITY_H_

#include <cstddef>
#include <sstream>
#include <string>
#include <utility>

namespace Socket
{

template<typename... Args>
int Print(std::ostream& s, Args&... args) {
    using Expander = int[];
    return Expander{ 0, ((s << std::forward<Args>(args)), 0)...}[0];
}

template<typename... Args>
std::string BuildStringFromParts(Args const&... args) {
    std::stringstream msg;
    Print(msg, args...);
    return msg.str();
}

template<typename... Args>
std::string BuildErrorMessage(Args const&... args) {
    return BuildStringFromParts(args...);
}

}

#endif // COMPONENTS_TCP_SOCKET_UTILITY_H_
