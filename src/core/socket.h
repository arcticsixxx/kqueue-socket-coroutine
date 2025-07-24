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
    accept_awaitable asyncAccept() { return accept_awaitable{event_loop_, socket_data_}; }
    read_awaitable asyncRead(int fd) { return read_awaitable{event_loop_, fd}; }

private:
    void setSocketData(const endpoint& ep);

protected:
    void openFileDescriptor();
    // returns fd of accepted socket
    socket_data accept();
    const char* read();

protected:
    EventLoop& event_loop_;
    socket_data socket_data_;
};
