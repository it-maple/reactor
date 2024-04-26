#pragma once

#include <functional>

#include "buffer.hpp"
#include "channel.hpp"
#include "inet_addr.hpp"
#include "uncopyable.hpp"

namespace reactor {

class TcpConnection : Uncopyable
{
public:
    using CordonCallback                    = std::function<void()>;
    using WriteCompleteCallback             = std::function<void()>;
    using WriteCompleteCallbackWithParam    = std::function<void(size_t, char const * const)>;
    using MessageCallback                   = std::function<void()>;
    using MessageCallbackWithParam          = std::function<void(size_t, const char * const)>;

    constexpr static const size_t CORDON    = 64 * 1024;
    constexpr static const size_t SEND_ZEROCOPY_PROPER_SIZE
                                            = 10 * 1024;

public:
    TcpConnection(Channel * const lhs);
    ~TcpConnection();

    Channel * const GetChannel();

    static void SetCordonCallback(CordonCallback const & cb);

    static void SetWriteCompleteCallback(WriteCompleteCallback const & cb);

    static void SetWriteCompleteCallbackWithParam(WriteCompleteCallbackWithParam const & cb);

    static void SetMessageCallback(MessageCallback const & cb);

    static void SetMessageCallbackWithParam(MessageCallbackWithParam const & cb);

    InetAddr GetPeerAddr();

    void Shutdown();

    void Send(ArrayBuffer & rhs);

    void Send(void * const buf, size_t len);

    void SendWithZeroCopy(int sockfd, void * const buf, size_t len);

private:
    void HandleRead();

    void HandleWrite();

    void HandleClose();

private:
    Channel *               channel_;

    ArrayBuffer             read_buf_;
    ArrayBuffer             write_buf_;

    static CordonCallback                  cordon_cb_;
    static WriteCompleteCallback           write_cpl_cb_;
    static WriteCompleteCallbackWithParam  write_cpl_arg_cb_;
    static MessageCallback                 msg_cb_;
    static MessageCallbackWithParam        msg_arg_cb_;
};

} // namespace