#pragma once

#include <stddef.h>
#include <stdint.h>
#include <sys/epoll.h>

#include <functional>
#include <vector>

#include "channel.hpp"
#include "uncopyable.hpp"

namespace reactor {

class Epoller : Uncopyable
{
public:
    constexpr static uint32_t READABLE      = EPOLLIN;
    constexpr static uint32_t WRITABLE      = EPOLLOUT;
    constexpr static uint32_t CLOSABLE      = EPOLLERR | EPOLLHUP | EPOLLRDHUP;
    constexpr static uint32_t DEFAULT_EVENT = READABLE | CLOSABLE | EPOLLET;

    constexpr static size_t MAX_EVENTS      = 1024;

public:
    using RegisterCallback  = std::function<void(Channel *)>;
    using DeleteCallback    = std::function<void(int)>;

public:
    Epoller();
    ~Epoller();

    RegisterCallback GetRegisterCallback() const;

    DeleteCallback GetDeleteCallback() const;

    static void SetRegisterCallback(RegisterCallback const & cb);

    static void SetDeleteCallback(DeleteCallback const & cb);

    void Register(int sockfd);

    void Modify(Channel * rhs);

    void Delete(int sockfd);

    void Delete(Channel * rhs);

    std::vector<Channel *> Wait(int timeout = 0);

private:
    int                 epfd_;
    static RegisterCallback    register_cb_;
    static DeleteCallback      delete_cb_;
};

} // namespace