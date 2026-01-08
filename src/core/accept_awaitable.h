#pragma once
#include <sys/event.h>
#include <unistd.h>
#include <cerrno>

#include <coroutine>

#include "event_loop.h"
#include "exceptions.h"
#include "types.h"

struct accept_awaitable {
    accept_awaitable(event_loop& event_loop, int fd)
        : event_loop_(event_loop)
        , fd_(fd) {
        data_.fd = -1;
        data_.addr_len = sizeof(data_.addr);
    }

    bool await_ready() noexcept {
        try_accept();
        return data_.fd >= 0;
    }

    void await_suspend(std::coroutine_handle<> handle) noexcept {
        if (!event_loop_.add_handle({handle, event::handle_type::ACCEPT, fd_})) {
            return;
        }
    }

    socket_data await_resume() {
        if (data_.fd >= 0) {
            return data_;
        }

        try_accept();
        if (data_.fd < 0) {
            throw accept_exception{errno};
        }

        return data_;
    }

private:
    void try_accept() noexcept {
        data_.addr_len = sizeof(data_.addr);
        data_.fd = ::accept(fd_,
            reinterpret_cast<struct sockaddr*>(&data_.addr),
            &data_.addr_len);
    }

    socket_data data_;
    event_loop& event_loop_;
    int fd_;
};
