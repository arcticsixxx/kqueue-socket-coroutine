#pragma once

#include <sys/event.h>
#include <unistd.h>

#include <coroutine>

#include "event_loop.h"
#include "exceptions.h"
#include "types.h"

struct accept_awaitable {
    accept_awaitable(EventLoop& event_loop, int fd)
        : event_loop_(event_loop)
        , fd_(fd) {}

    bool await_ready() const noexcept { return false; }

    void await_suspend(std::coroutine_handle<> handle) noexcept {
        struct kevent kev;
        EV_SET(&kev, fd_, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, nullptr);
        kevent(event_loop_.getKq(), &kev, 1, nullptr, 0, nullptr);

        event_loop_.addHandle(fd_, EventLoop::handle_type::ACCEPT, handle);
    }

    socket_data await_resume() const {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        int client_fd = ::accept(fd_,
            reinterpret_cast<struct sockaddr*>(&client_addr),
            &client_len);

        if (client_fd < 0) {
            throw accept_exception{client_fd};
        }

        return { client_addr, client_len, client_fd };
    }

private:
    EventLoop& event_loop_;
    int fd_;
};

template <typename Container>
requires Buffer<Container>
struct read_awaitable
{
    read_awaitable(EventLoop& event_loop, Container& buf, int fd)
        : event_loop_(event_loop)
        , buf_(buf)
        , fd_(fd) {}

    bool await_ready() const noexcept { return false; }

    void await_suspend(std::coroutine_handle<> handle) noexcept {
        struct kevent kev;
        EV_SET(&kev, fd_, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, nullptr);
        kevent(event_loop_.getKq(), &kev, 1, nullptr, 0, nullptr);

        event_loop_.addHandle(fd_, EventLoop::handle_type::READ, handle);
    }

    size_t await_resume() const {
        size_t nbytes = ::read(fd_, std::ranges::data(buf_), buf_.size());

        // eof
        if (nbytes == 0) {
            event_loop_.removeHandle(fd_);
            return 0;
        }

        if (nbytes < 0) {
            throw read_exception{errno};
        }

        return static_cast<size_t>(nbytes);
    }

private:
    EventLoop& event_loop_;
    Container& buf_;
    int fd_;
};

struct promise;
struct coro_task : std::coroutine_handle<promise>
{
    using promise_type = ::promise;
};
struct promise
{
    coro_task get_return_object() { return coro_task{}; }
    std::suspend_never initial_suspend() noexcept { return {}; }
    std::suspend_never final_suspend() noexcept { return {}; }
    void return_void() {}
    void unhandled_exception() { std::rethrow_exception(std::move(std::current_exception())); }
};
