#pragma once

#include <cstddef>
#include <sys/event.h>

#include <coroutine>
#include <vector>

class event_loop {
public:
    enum class handle_type {
        ACCEPT = 0,
        READ,
        WRITE
    };

    explicit event_loop();

    event_loop(const event_loop&) = delete;
    event_loop& operator=(const event_loop&) = delete;

    ~event_loop() noexcept;

    void run() { loop(); }
    void stop() {};

    inline int get_kq() const noexcept { return kq; }
    inline void add_handle(int fd, handle_type type, std::coroutine_handle<> handle) {
        handlers.emplace_back(event{handle, type, fd});
    }

    inline void remove_handle(int fd) {
        std::erase_if(handlers, [fd](const auto& e) { return e.fd == fd; });
    }

private:
    void loop();

    struct event {
        std::coroutine_handle<> handle;
        handle_type type;
        int fd;
    };

    std::vector<event> handlers;
    std::vector<struct kevent> events;
    int kq;
};
