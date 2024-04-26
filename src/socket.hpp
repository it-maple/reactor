#pragma once

#include <string>

#include "inet_addr.hpp"

namespace reactor {

class Socket
{
    public:
        explicit Socket(int sockfd);
        Socket(std::string & ip, std::string & port);
        Socket(const Socket&) = default;
        Socket(Socket&&) = default;
        Socket& operator=(const Socket&) = default;
        Socket& operator=(Socket&&) = default;
        ~Socket();

        const int & operator()() noexcept;

        int BindAddr();

        int BindAddr(std::string const & ip, std::string const & port);

        static int BindAddr(int sockfd, std::string const & ip, std::string const & port);

        int Listen();

        static int Listen(int fd, int backlog = 50);

        int Accept();

        void ReuseAddrPort();

        void Close();

        static void Close(int fd);

        static int ConnectTo(InetAddr addr);

        static ssize_t Read(int sockfd, void * const buf, size_t count);

        static ssize_t Write(int sockfd, void const * buf, size_t count);

        static void MakeNonblocking(int fd);

        static InetAddr GetPeerAddr(int fd);

    private:
        int         sockfd_;
        std::string ip_;
        std::string port_;
};

} // namespace reactor