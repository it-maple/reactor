#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "tcp_conn.hpp"
#include "socket.hpp"

namespace reactor {

TcpConnection::TcpConnection(Channel * const lhs)
    : channel_(lhs)
    , read_buf_()
    , write_buf_()
{
    channel_->SetReadCallback(  std::bind(&TcpConnection::HandleRead,  this));
    channel_->SetWroteCallback( std::bind(&TcpConnection::HandleWrite, this));
    channel_->SetClosedCallback(std::bind(&TcpConnection::HandleClose, this));
}

TcpConnection::~TcpConnection()
{
    if (channel_)
        channel_->DisableAll();
}

Channel * const TcpConnection::GetChannel()
{
    return channel_;
}

void TcpConnection::SetCordonCallback(
    CordonCallback const & cb)
{
    cordon_cb_ = std::move(cb);
}

void TcpConnection::SetWriteCompleteCallback(
    WriteCompleteCallback const & cb)
{
    write_cpl_cb_ = std::move(cb);
}

void TcpConnection::SetWriteCompleteCallbackWithParam(
    WriteCompleteCallbackWithParam const & cb)
{
    write_cpl_arg_cb_ = std::move(cb);
}

void TcpConnection::SetMessageCallback(
    MessageCallback const & cb)
{
    msg_cb_ = std::move(cb);
}

void TcpConnection::SetMessageCallbackWithParam(
    MessageCallbackWithParam const & cb)
{
    msg_arg_cb_ = std::move(cb);
}

InetAddr TcpConnection::GetPeerAddr()
{
    return Socket::GetPeerAddr(channel_->SockFd());
}

void TcpConnection::Shutdown()
{
    if (!channel_->Writable())
    {
        channel_->DisableWrite();
        shutdown(channel_->SockFd(), SHUT_WR);
    }
}

void TcpConnection::Send(ArrayBuffer & rhs)
{
    Send(rhs.peek(), rhs.readableBytes());
}

void TcpConnection::Send(void * const buf, size_t len)
{
    size_t  remain  = len;
    ssize_t wrote   = 0;
    if (!channel_->Writable() && write_buf_.readableBytes() == 0)
    {
        wrote = Socket::Write(channel_->SockFd(), buf, len);
        if (wrote >= 0)
        {
            remain = len - wrote;
            if (remain == 0 && write_cpl_cb_)
                write_cpl_cb_();
        }
        else
        {
            perror("write(): ");
            wrote = 0;
            if (errno == EAGAIN || errno == EWOULDBLOCK)
                channel_->DisableWrite();
            else
                return;
        }
    }

    if (remain > 0)
    {
        size_t rest_len = write_buf_.readableBytes();
        if (rest_len + remain > CORDON
            && rest_len < CORDON
            && cordon_cb_)
            cordon_cb_();
        write_buf_.append(static_cast<char *>(buf) + wrote, remain);
        if (!channel_->Writable())
            channel_->EnableWrite();
    }
}

void TcpConnection::SendWithZeroCopy(int sockfd, void * const buf, size_t len)
{
    size_t size_for_send = 0;
    if (len > SEND_ZEROCOPY_PROPER_SIZE)
        size_for_send = SEND_ZEROCOPY_PROPER_SIZE;
    else
        size_for_send = len;

    auto sent = send(sockfd, buf, size_for_send, MSG_ZEROCOPY);
    if (sent == -1)
    {
        perror("send() with MSG_ZEROCOPY flag: ");
        return;
    }
    if (static_cast<size_t>(sent) == size_for_send
        && size_for_send == SEND_ZEROCOPY_PROPER_SIZE)
    {
        if (write_cpl_cb_)
            write_cpl_cb_();
        if (static_cast<size_t>(sent) > CORDON)
            cordon_cb_();
    }
    else
    {
        write_buf_.append(static_cast<char *>(buf) + sent, len - sent);
        if (!channel_->Writable())
            channel_->EnableWrite();
    }
}

void TcpConnection::HandleRead()
{
    ssize_t n = Socket::Read(channel_->SockFd(), read_buf_.peek(), read_buf_.writableBytes());
    if (n > 0)
    {
        if (msg_cb_)
            msg_cb_();
        else if (msg_arg_cb_)
            msg_arg_cb_(n, read_buf_.retrieveAllAsString().c_str());

        return;
    }
    else if (n == 0)
        HandleClose();
    else
        perror("read(): ");
}

void TcpConnection::HandleWrite()
{
    if (channel_->Writable())
    {
        ssize_t n = Socket::Write(channel_->SockFd(),
                  write_buf_.peek(),
                write_buf_.readableBytes());
        if (n > 0)
        {
            if (write_buf_.readableBytes() == 0)
            {
                write_buf_.retrieve(n);
                channel_->DisableWrite();
                return;
            }
            if (write_cpl_cb_)
            {
                write_cpl_cb_();
                write_buf_.retrieve(n);
            }
            else if (write_cpl_arg_cb_)
            {
                write_cpl_arg_cb_(n, write_buf_.retrieveAsString(n).c_str());
            }
        }
        else
            perror("write(): ");
    }
}

void TcpConnection::HandleClose()
{
    channel_->DisableAll();
}

} // namespace