#pragma once

#include <cstdio>
#include <functional>
#include <mutex>
#include <queue>
#include <sys/epoll.h>
#include <unistd.h>
#include <vector>

#include "timer.hpp"
#include "uncopyable.hpp"

namespace reactor {

class TimerQueue : public Uncopyable
{
public:
    TimerQueue()
    : epfd_(-1)
    , stop_(false)
    {
        epfd_ = epoll_create(1);
        if (epfd_ < 0)
        {
            perror("epoll_create(): ");
            return;
        }
    }

    ~TimerQueue()
    {
        if (epfd_ > 0)
            close(epfd_);
    }

    void AddTimer(Timer const & rhs)
    {
        auto & t = queue_.top();
        if (rhs > t)
            const_cast<Timer &>(rhs).SetPeriod(rhs.Period() - t.Period());
        else if (rhs.Period() == t.Period())
            const_cast<Timer &>(rhs).SetPeriod(1);

        std::lock_guard<std::mutex> guard(mx_);
        queue_.push(rhs);
    }

    void Stop() { stop_ = true; }

    void Execute()
    {
        struct epoll_event events;
        while (!stop_)
        {
            auto & timer = queue_.top();
            auto period = timer.Period();
            // sleep for period milliseconds
            epoll_wait(epfd_, (struct epoll_event *) &events, 1, period);
            // execute task after being waken up
            auto & t = const_cast<Timer &>(timer);
            t();
            // re-append task
            auto new_t = std::move(t);
            std::lock_guard<std::mutex> guard(mx_);
            queue_.pop();
            queue_.push(new_t);
        }
    }

private:
    std::mutex                  mx_;
    volatile int                epfd_;
    volatile bool               stop_;
    std::priority_queue<Timer,
        std::vector<Timer>,
        std::greater<Timer>>    queue_;
};

} // namespace