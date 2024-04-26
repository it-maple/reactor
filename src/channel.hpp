#pragma once

#include <cstdint>
#include <functional>

namespace reactor {

class Epoller;

class Channel
{
public:
    using ReadCallback      = std::function<void()>;
    using WroteCallback     = std::function<void()>;
    using ClosedCallback    = std::function<void(int)>;

public:
    Channel(int sockfd, Epoller * epoller);
    Channel(const Channel &) = default;
    Channel(Channel &&) = default;
    Channel & operator=(const Channel &) = default;
    Channel & operator=(Channel &&) = default;
    ~Channel();

    static void SetReadCallback(ReadCallback const & cb);

    static void SetWroteCallback(WroteCallback const & cb);

    static void SetClosedCallback(ClosedCallback const & cb);

    int SockFd() const;

    void SetEvents(uint32_t rhs);

    void SetRevents(uint32_t rhs);

    uint32_t Events() const;

    uint32_t Revents() const;

    void EnableRead();

    void EnableWrite();

    void DisableRead();
    
    void DisableWrite();

    bool Readable();

    bool Writable();

    bool Closable();

    void DisableAll();

    void HandleEvents();

private:
    int             sockfd_;
    Epoller *       epoller_;
    uint32_t        events_;
    uint32_t        revents_;

    static ReadCallback    read_cb_;
    static WroteCallback   wrote_cb_;
    static ClosedCallback  closed_cb_;
};

} // namespace