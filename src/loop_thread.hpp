#pragma once

#include <thread>

#include "event_loop.hpp"
#include "uncopyable.hpp"

namespace reactor {

class LoopThread : public Uncopyable
{
public:
    LoopThread()
        : loop_(new EventLoop)
        , t_(std::bind(&LoopThread::Func, this))
    {}

    LoopThread(LoopThread && rhs)
        : loop_(rhs.loop_)
        , t_(std::move(rhs.t_))
    {
        rhs.loop_ = nullptr;
    }

    ~LoopThread()
    {
        if (loop_)
            delete loop_;

        t_.join();
    }

    EventLoop * const Loop() { return loop_; }

    void Stop() { stop_ = true; }

private:
    void Func()
    {
        while (!stop_)
        {
            loop_->Loop();
        }
    }

private:
    volatile bool       stop_;
    EventLoop *         loop_;
    std::thread         t_;
};

} // namespace