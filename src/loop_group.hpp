#pragma once

#include <cassert>
#include <vector>

#include "loop_thread.hpp"
#include "uncopyable.hpp"

namespace reactor {

class LoopThreadGroup : Uncopyable
{
public:
    LoopThreadGroup(bool notify = true)
        : loop_t_()
    {}

    ~LoopThreadGroup()
    {
        Stop();
    }

    void Init(int n)
    {
        assert(n >= 1);
        for (int i = 0; i < n; i++)
        {
            loop_t_.emplace_back();
        }
    }

    void RegisterFd(int fd)
    {
        static auto iter = loop_t_.begin();
        (*iter).Loop()->RegisterFd(fd);
        ++iter;
        if (iter._M_is_end())
            iter = loop_t_.begin();
    }

    void Stop()
    {
        for (auto & loop : loop_t_)
            loop.Stop();
    }

private:
    std::vector<LoopThread>                     loop_t_;
};

} // namespace reactor