#include "event_loop.hpp"

namespace reactor {

EventLoop::EventLoop()
    : stop_(false)
    , epoller_(new Epoller)
    {}

EventLoop::~EventLoop()
{
    if (epoller_)
        delete epoller_;
}

void EventLoop::Stop() { stop_ = true; }

void EventLoop::Loop()
{
    while (!stop_)
    {
        auto ret = epoller_->Wait();
        for (auto & ptr : ret)
            ptr->HandleEvents();

        DoPendingFunctor();
    }
}

void EventLoop::RegisterFd(int sockfd)
{
    epoller_->Register(sockfd);
}

void EventLoop::DoPendingFunctor()
{
    std::vector<Functor> functores;
    {
        std::lock_guard<std::mutex> guard(mx_);
        functores.swap(functor_);
    }
    for (auto & f : functores)
        f();
}

void EventLoop::RunInLoop(Functor const  & f)
{
    std::lock_guard<std::mutex> guard(mx_);
    functor_.push_back(std::move(f));
}

} // namespace