#include <netinet/in.h>
#include <sys/socket.h>

#include "acceptor.hpp"

namespace reactor {

Acceptor::Acceptor(int sockfd)
    : sockfd_(sockfd)
    , accepted_cb_()
    {}

Acceptor::~Acceptor() {}

void Acceptor::SetAcceptedCallback(AcceptedCallback cb)
{
    accepted_cb_ = cb;
}

int Acceptor::Accept()
{
    socklen_t len;
    struct sockaddr_in addr;

    auto ret = accept(sockfd_, reinterpret_cast<struct sockaddr *>(&addr), &len);
    if (ret > 0 && accepted_cb_)
        accepted_cb_(ret);

    return ret;
}

} // namespace