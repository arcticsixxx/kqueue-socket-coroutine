#pragma once

#include <sys/socket.h>

#include "accept_awaitable.h"
#include "event_loop.h"
#include "read_awaitable.h"
#include "types.h"

class Socket {
public:
    Socket(event_loop& event_loop) noexcept;
    Socket(event_loop& event_loop, socket_data&& socket_data);
    Socket(event_loop& event_loop, const endpoint& ep);

    Socket(Socket&& other) noexcept : event_loop_(other.event_loop_){
        socket_data_ = other.socket_data_;
        other.socket_data_.fd = -1;
    }

    auto operator=(Socket&& other) noexcept -> Socket& {
        if (this == &other) {
            return *this;
        }

        if (socket_data_.fd >= 0) {
            close(socket_data_.fd);
        }

        socket_data_ = other.socket_data_;
        other.socket_data_.fd = -1;

        return *this;
    }

    ~Socket() noexcept;

    Socket(const Socket&) = delete;
    auto operator=(const Socket&) -> Socket& = delete;

    const int& get_fd() const {
        return socket_data_.fd;
    }

    void listen(size_t max_connections = 32);
    void bind(const endpoint& ep);
    auto async_accept() -> accept_awaitable {
        return accept_awaitable{event_loop_, socket_data_.fd };
    }

    template <Buffer Container>
    auto async_read(int fd, Container& buffer) -> read_awaitable<Container> {
        return read_awaitable<Container>{event_loop_, buffer, fd};
    }

    socket_data accept();
    const char* read();

private:
    void open_file_descriptor();
    void set_nonblock();
    void set_socket_data(const endpoint& ep);

    event_loop& event_loop_;
    socket_data socket_data_;
};
