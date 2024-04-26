#pragma once

#include <chrono>
#include <functional>
#include <thread>

#include "uncopyable.hpp"

namespace reactor {

class Timer : public Uncopyable
{
public:
    using Task = std::function<void()>;
public:
    Timer()
        : period_(-1)
        , executed_(false)
        , async_(false)
        , task_()
    {}
    Timer(int period, Task const & task)
        : period_(period)
        , executed_(false)
        , async_(false)
        , task_(std::move(task))
    {}
    Timer(Timer && rhs) = default;
    ~Timer() {}

    void operator()()
    {
        if (!executed_)
            PeriodTask(period_, task_);
    }

    friend bool operator<(Timer const & lhs, Timer const & rhs)
    {
        return lhs.period_ < rhs.period_;
    }

    friend bool operator>(Timer const & lhs, Timer const & rhs)
    {
        return lhs.period_ > rhs.period_;
    }

    int Period() const { return period_; }

    void SetPeriod(int rhs) { period_ = rhs; }

    void SetAsync(bool rhs) { async_ = rhs; }

    void SetTask(Task const & rhs) { task_ = std::move(rhs); }

    void PeriodTask(int interval, Task const & task, bool async = false)
    {
        async_      = async;
        executed_   = true;
        if (async_)
        {
            std::thread([&] {
                while (executed_)
                {
                    task();
                    std::this_thread::sleep_for(
                        std::chrono::milliseconds(interval));
                }
            }).detach();
        }
        else
        {
            while (executed_)
            {
                task();
                std::this_thread::sleep_for(
                    std::chrono::milliseconds(interval));
            }
        }
    }

    void Stop() { executed_ = false; }

private:
    volatile int    period_;
    volatile bool   executed_;
    volatile bool   async_;
    Task            task_;
};

} // namespace