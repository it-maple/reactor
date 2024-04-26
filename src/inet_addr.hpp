#pragma once

#include <cstdint>
#include <string>

namespace reactor {

class InetAddr
{
public:
    InetAddr(std::string ip, uint16_t port);
    InetAddr(const InetAddr &) = default;
    InetAddr(InetAddr &&) = default;
    InetAddr & operator=(const InetAddr &) = default;
    InetAddr & operator=(InetAddr &&) = default;
    ~InetAddr() = default;

    inline bool operator==(InetAddr const & addr);

    inline bool operator==(InetAddr && addr);

    // [ip:port]
    std::string ToString();

    inline std::string operator()();

    std::string IP();

    std::string Port();

private:
    std::string ip_;
    uint16_t    port_;
};

} // namespace