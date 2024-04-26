#include <unistd.h>

#include "channel.hpp"
#include "epoller.hpp"
#include "socket.hpp"

namespace reactor {

Channel::Channel(int sockfd, Epoller * epoller)
    : sockfd_(sockfd)
    , epoller_(epoller)
    , events_(Epoller::DEFAULT_EVENT)
    , revents_(Epoller::DEFAULT_EVENT)
    {}

Channel::~Channel()
{
    DisableAll();
}

void Channel::SetReadCallback(Channel::ReadCallback const & cb)
{
    read_cb_ = std::move(cb);
}

void Channel::SetWroteCallback(Channel::WroteCallback const & cb)
{
    wrote_cb_ = std::move(cb);
}

void Channel::SetClosedCallback(Channel::ClosedCallback const & cb)
{
    closed_cb_ = std::move(cb);
}

int Channel::SockFd() const
{
    return sockfd_;
}

uint32_t Channel::Events() const
{
    return events_;
}

uint32_t Channel::Revents() const
{
    return revents_;
}

void Channel::SetEvents(uint32_t rhs)
{
    events_ = rhs;
}

void Channel::SetRevents(uint32_t rhs)
{
    revents_ = rhs;
}

void Channel::EnableRead()
{
    events_ = Epoller::DEFAULT_EVENT;
    epoller_->Modify(this);
}

void Channel::EnableWrite()
{
    events_ |= Epoller::WRITABLE;
    epoller_->Modify(this);
}

void Channel::DisableRead()
{
    events_ &= (~Epoller::READABLE);
    epoller_->Modify(this);
}

void Channel::DisableWrite()
{
    events_ &= (~Epoller::WRITABLE);
    epoller_->Modify(this);
}

bool Channel::Readable()
{
    return revents_ & Epoller::READABLE;
}

bool Channel::Writable()
{
    return revents_ & Epoller::WRITABLE;
}

bool Channel::Closable()
{
    return revents_ & Epoller::CLOSABLE;
}

void Channel::DisableAll()
{
    Socket::Close(sockfd_);
    // epoller_->Delete(sockfd_);
    if (epoller_)
        epoller_->Delete(this);
}

void Channel::HandleEvents()
{
    if (revents_ & Epoller::READABLE)
    {
        if (read_cb_)
            read_cb_();
    }
    if (revents_ & Epoller::WRITABLE)
    {
        if (wrote_cb_)
            wrote_cb_();
    }
    if (revents_ & Epoller::CLOSABLE)
    {
        if (closed_cb_)
            closed_cb_(sockfd_);
    }
}

} //CLOSADABLE