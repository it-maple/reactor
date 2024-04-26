#include "tcp_server.hpp"
#include "socket.hpp"
#include "tcp_conn.hpp"

namespace reactor {

TcpServer::TcpServer(std::string ip, std::string port, int n_loop)
    : stop_(false)
    , server_(ip, port)
    , acceptor_(server_())
    , loop_group_(false)
{
    Socket::MakeNonblocking(server_());
    loop_group_.Init(n_loop);

    Epoller::SetRegisterCallback(std::bind(
                                    &TcpServer::EmplaceConnection,
                                    this,
                                    std::placeholders::_1));
    Epoller::SetDeleteCallback(std::bind(
                                    &TcpServer::EraseConnection,
                                    this,
                                    std::placeholders::_1));
}

TcpServer::~TcpServer()
{
    for (auto & pair : conn_map_)
    {
        Socket::Close(pair.first);
        pair.second.reset();
    }
}

void TcpServer::EmplaceConnection(Channel * const rhs)
{
    std::lock_guard<std::mutex> guard(mx_);
    conn_map_.emplace(rhs->SockFd(),
                      new TcpConnection(rhs));
}

void TcpServer::EraseConnection(int sockfd)
{
    std::lock_guard<std::mutex> guard(mx_);
    conn_map_.erase(sockfd);
}

void TcpServer::Start()
{
    while (!stop_)
    {
        auto fd = acceptor_.Accept();
        if (fd > 0)
            loop_group_.RegisterFd(fd);
    }
}

void TcpServer::Stop()
{
    stop_ = true;
}

void TcpServer::SetWCbWP(size_t n, char const * const buf)
{
    TcpConnection::SetWriteCompleteCallbackWithParam(on_wwp_);
}

void TcpServer::SetRCbWP(size_t n, char const * const buf)
{
    TcpConnection::SetMessageCallbackWithParam(on_rwp_);
}
} // namespace