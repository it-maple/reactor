#include <arpa/inet.h>
#include <assert.h>
#include <cstring>
#include <errno.h>
#include <fcntl.h>
#include <stdexcept>
#include <unistd.h>

#include "socket.hpp"

namespace reactor {

Socket::Socket(int sockfd)
    : sockfd_(sockfd)
    , ip_()
    , port_()
{
    assert(sockfd >= 0);
}

Socket::Socket(std::string & ip, std::string & port)
    : sockfd_(::socket(AF_INET, SOCK_STREAM, 0))
    , ip_(ip)
    , port_(port)
{
    assert(sockfd_ >= 0);

    BindAddr();
}

Socket::~Socket()
{
    close(sockfd_);
}

const int & Socket::operator()() noexcept { return sockfd_; }

int Socket::BindAddr()
{
    return BindAddr(ip_, port_);
}

int Socket::BindAddr(std::string const & ip, std::string const & port)
{
    return BindAddr(sockfd_, ip, port);
}

int Socket::BindAddr(int sockfd, std::string const & ip, std::string const & port)
{
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    inet_aton(ip.c_str(), &addr.sin_addr);
    addr.sin_port = htons(::atoi(port.c_str()));

    auto ret = bind(sockfd, (struct sockaddr*) &addr, sizeof(addr));
    assert(ret != -1);

    return ret;
}

int Socket::Listen()
{
    auto ret = listen(sockfd_, 50);
    assert(ret != -1);

    return ret;
}

int Socket::Listen(int fd, int backlog)
{
    auto ret = listen(fd, 50);
    assert(ret != -1);

    return ret;
}

int Socket::Accept()
{
    socklen_t addrLen;
    struct sockaddr_in clientAddr;
    auto ret = accept(sockfd_, (struct sockaddr *) &clientAddr, &addrLen);
    
    if (errno == EAGAIN || errno == EWOULDBLOCK)
        return ret;

    assert(ret != -1);

    return ret;
}

void Socket::Close()
{
    close(sockfd_);
}

void Socket::Close(int fd)
{
    close(fd);
}

void Socket::ReuseAddrPort()
{
    int reuse = 1;
    auto ret = ::setsockopt(sockfd_, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
    assert(ret != -1);
    ret = setsockopt(sockfd_, SOL_SOCKET, SO_REUSEPORT, &reuse, sizeof(reuse));
    assert(ret != -1);
}

int Socket::ConnectTo(InetAddr addr)
{
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr(addr.IP().c_str());
    serverAddr.sin_port = htons(atoi(addr.Port().c_str()));

    auto sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
        throw std::runtime_error("can't create new socket");

    auto ret = connect(sock, (struct sockaddr *) &serverAddr, sizeof(serverAddr));
    if (ret < 0)
        throw std::runtime_error("can not establish connection");

    return sock;
}

ssize_t Socket::Read(int sockfd, void * const buf, size_t count)
{
    return read(sockfd, buf, count);
}

ssize_t Socket::Write(int sockfd, const void *buf, size_t count)
{
    return write(sockfd, buf, count);
}

void Socket::MakeNonblocking(int fd)
{
    auto flag = fcntl(fd, F_GETFL, 0);
    fcntl(fd, flag | O_NONBLOCK);
}

InetAddr Socket::GetPeerAddr(int fd)
{
    struct sockaddr_in client_addr; 
    socklen_t len = sizeof(client_addr);
    getpeername(fd, (struct sockaddr *) (&client_addr), &len);

    return InetAddr(
            inet_ntoa(client_addr.sin_addr), 
            ntohs(client_addr.sin_port));
}

} // namespace reactor
