#pragma once

#include <functional>

#include "uncopyable.hpp"

namespace reactor {

class Acceptor : Uncopyable
{
public:
    using AcceptedCallback = std::function<void(int)>;

public:
    Acceptor(int sockfd);
    ~Acceptor();

    void SetAcceptedCallback(AcceptedCallback cb);

    int Accept();

private:
    int                 sockfd_;
    AcceptedCallback    accepted_cb_;
};

} // namespace