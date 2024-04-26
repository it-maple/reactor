#include <cassert>
#include <unistd.h>

#include "epoller.hpp"

namespace reactor {

Epoller::Epoller()
    : epfd_(epoll_create(1024))
{
    assert(epfd_ != -1);
}

Epoller::~Epoller()
{
    close(epfd_);
}

Epoller::RegisterCallback Epoller::GetRegisterCallback() const
{
    return register_cb_;
}

Epoller::DeleteCallback Epoller::GetDeleteCallback() const
{
    return delete_cb_;
}

void Epoller::SetRegisterCallback(RegisterCallback const & cb)
{
    register_cb_ = cb;
}

void Epoller::SetDeleteCallback(DeleteCallback const & cb)
{
    delete_cb_ = std::move(cb);
}

void Epoller::Register(int sockfd)
{

    auto channel = new Channel(sockfd, this);
    channel->SetEvents(DEFAULT_EVENT);

    struct epoll_event event;
    event.events = DEFAULT_EVENT;
    event.data.ptr = static_cast<void *>(channel);

    if (register_cb_)
        register_cb_(channel);

    epoll_ctl(epfd_, EPOLL_CTL_ADD, sockfd, &event);
}

void Epoller::Modify(Channel * rhs)
{
    struct epoll_event event;
    event.events = rhs->Events();
    event.data.ptr = static_cast<void *>(rhs);

    epoll_ctl(epfd_, EPOLL_CTL_MOD, rhs->SockFd(), &event);
}

inline void Epoller::Delete(int sockfd)
{
    epoll_ctl(epfd_, EPOLL_CTL_DEL, sockfd, nullptr);

    if (delete_cb_)
        delete_cb_(sockfd);
}


inline void Epoller::Delete(Channel * rhs)
{
    if (rhs)
    {
        epoll_ctl(epfd_, EPOLL_CTL_DEL, rhs->SockFd(), nullptr);
        if (delete_cb_)
            delete_cb_(rhs->SockFd());
        delete rhs;
    }
}

std::vector<Channel *> Epoller::Wait(int timeout)
{
    struct epoll_event events[MAX_EVENTS];

    auto ret = epoll_wait(epfd_, events, MAX_EVENTS, timeout);
    if (ret)
        return std::vector<Channel *>();

    std::vector<Channel *> resp(ret);
    for (int i = 0; i < ret; ++i)
    {
        auto ptr = static_cast<Channel *>(events[i].data.ptr);
        ptr->SetRevents(events[i].events);
        resp[i] = ptr;
    }

    return resp;
}

} // namespace