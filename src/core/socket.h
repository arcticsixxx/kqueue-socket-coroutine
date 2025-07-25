#pragma once

#include <sys/socket.h>

#include "coroutine_ops.h"
#include "event_loop.h"
#include "types.h"

class Socket {
public:
    Socket(EventLoop& event_loop) noexcept;
    Socket(EventLoop& event_loop, const endpoint& ep);
    ~Socket() noexcept;

    void listen();
    void bind(const endpoint& ep);
    socket_data accept();
    accept_awaitable asyncAccept() { return accept_awaitable{event_loop_, socket_data_.fd }; }
    const char* read();

    template <Buffer Container>
    read_awaitable<Container> asyncRead(int fd, Container& buffer) { return read_awaitable<Container>{event_loop_, buffer, fd}; }

private:
    void openFileDescriptor();
    void setSocketData(const endpoint& ep);

protected:
    EventLoop& event_loop_;
    socket_data socket_data_;
};
