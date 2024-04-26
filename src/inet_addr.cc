#include "inet_addr.hpp"

namespace reactor {

InetAddr::InetAddr(std::string ip, uint16_t port)
    : ip_(ip)
    , port_(port) {}

inline bool InetAddr::operator==(InetAddr const & addr)
{
    return addr.ip_ == ip_ && addr.port_ == port_;
}

inline bool InetAddr::operator==(InetAddr && addr)
{
    return addr.ip_ == ip_ && addr.port_ == port_;
}

std::string InetAddr::ToString()
{
    // [ip:port]
    std::string info;
    auto port = std::to_string(port_);

    info.append("[");
    info.append(ip_.c_str());
    info.append(":");

    info.append(port.c_str());
    info.append("]");

    return info;
}

inline std::string InetAddr::operator()()
{
    return ToString();
}

std::string InetAddr::IP() { return ip_; }

std::string InetAddr::Port() { return std::to_string(port_); }

} // namespace