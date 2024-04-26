#pragma once

#include <functional>
#include <mutex>
#include <vector>

#include "epoller.hpp"
#include "uncopyable.hpp"

namespace reactor {

class EventLoop : public Uncopyable
{
public:
    using Functor = std::function<void()>;

public:
    EventLoop();
    ~EventLoop();

    void Stop();

    void Loop();

    void RegisterFd(int sockfd);

    void DoPendingFunctor();

    void RunInLoop(Functor const  & f);

private:
    std::mutex                      mx_;
    volatile bool                   stop_;
    Epoller *                       epoller_;
    std::vector<Functor>            functor_;
};

} // namespace