#pragma once

#include <netinet/in.h>
#include <string>

namespace Kagami {

enum K_HINTS { K_HINTS_DEVNEW, K_HINTS_EVNEW };

struct hint_header {
    enum K_HINTS hint;
};
namespace Net {

int addr_create(std::string ip, int port, struct sockaddr_in *address);

} // namespace Net

} // namespace Kagami
