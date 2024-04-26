#pragma once

#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>

#include "acceptor.hpp"
#include "loop_group.hpp"
#include "socket.hpp"
#include "tcp_conn.hpp"
#include "uncopyable.hpp"

namespace reactor {

class TcpServer : public Uncopyable
{
public:
    using OnCordon = TcpConnection::CordonCallback;
    using OnW = TcpConnection::WriteCompleteCallback;
    using OnWWP = TcpConnection::WriteCompleteCallbackWithParam;
    using OnR = TcpConnection::MessageCallback;
    using OnRWP = TcpConnection::MessageCallbackWithParam;

    using TcpConnectionPtr = std::unique_ptr<TcpConnection>;

public:
    TcpServer(std::string ip, std::string port, int n_loop);
    ~TcpServer();

    void EmplaceConnection(Channel * const rhs);

    void EraseConnection(int sockfd);

    void Start();

    void Stop();

    template<typename Func, typename ...Args>
    void SetCordonCb(Func && f, Args && ...args);

    template<typename Func, typename ...Args>
    void SetWCb(Func && f, Args && ...args);

    void SetWCbWP(size_t n, char const * const buf);

    template<typename Func, typename ...Args>
    void SetRCb(Func && f, Args && ...args);

    void SetRCbWP(size_t n, char const * const buf);

private:
    void CheckAlive();

private:
    volatile bool                           stop_;
    Socket                                  server_;
    Acceptor                                acceptor_;
    LoopThreadGroup                         loop_group_;
    std::unordered_map<int,
                       TcpConnectionPtr>    conn_map_;
    std::mutex                              mx_;

    OnCordon                                on_c_;
    OnW                                     on_w_;
    OnWWP                                   on_wwp_;
    OnR                                     on_r_;
    OnRWP                                   on_rwp_;
};

template<typename Func, typename ...Args>
    void TcpServer::SetCordonCb(Func && f, Args && ...args)
    {
        on_c_ = std::bind<decltype(f(args...))>(std::forward(f),
                    std::forward(args)...);
        TcpConnection::SetCordonCallback(on_c_);
    }

template<typename Func, typename ...Args>
    void TcpServer::SetWCb(Func && f, Args && ...args)
    {
        on_w_ = std::bind<decltype(f(args...))>(std::forward(f),
                    std::forward(args)...);
        TcpConnection::SetWriteCompleteCallback(on_w_);
    }

template<typename Func, typename ...Args>
    void TcpServer::SetRCb(Func && f, Args && ...args)
    {
        on_r_ = std::bind<decltype(f(args...))>(std::forward(f),
                    std::forward(args)...);
        TcpConnection::SetMessageCallback(on_r_);
    }

} // namespace