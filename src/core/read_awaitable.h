#pragma once

#include <sys/event.h>
#include <unistd.h>

#include <coroutine>

#include "event_loop.h"
#include "exceptions.h"
#include "types.h"

template <typename Container>
requires Buffer<Container>
struct read_awaitable
{
    read_awaitable(event_loop& event_loop, Container& buf, int fd)
        : event_loop_(event_loop)
        , buf_(buf)
        , fd_(fd) {}

    bool await_ready() const noexcept { return false; }

    void await_suspend(std::coroutine_handle<> handle) noexcept {
        event_loop_.add_handle({handle, event::handle_type::READ, fd_});
    }

    size_t await_resume() const {
        size_t nbytes = ::read(fd_, std::ranges::data(buf_), buf_.size());

        // eof
        if (nbytes == 0) {
            event_loop_.remove_handle(fd_);
            return 0;
        }

        if (nbytes < 0) {
            throw read_exception{errno};
        }

        return static_cast<size_t>(nbytes);
    }

private:
    event_loop& event_loop_;
    Container& buf_;
    int fd_;
};
