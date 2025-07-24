#pragma once

#include <cstddef>
#include <sys/event.h>

#include <array>
#include <coroutine>
#include <unordered_map>

class EventLoop {
public:
    enum class handle_type {
        ACCEPT = 0,
        READ
    };

    explicit EventLoop();

    EventLoop(const EventLoop&) = delete;
    EventLoop& operator=(const EventLoop&) = delete;

    ~EventLoop() noexcept;

    void run() { loop(); }
    void stop() {};

    inline int getKq() const noexcept { return kq; }
    inline void addHandle(int fd, handle_type type, std::coroutine_handle<>& handle) {
        handlers[fd] = handler{handle, type};
    }

    inline void removeHandle(int fd) {
        if (handlers.contains(fd)) {
            handlers.erase(fd);
        }
    }

private:
    void loop();

    struct handler {
        std::coroutine_handle<> handle;
        handle_type type;
    };

    std::array<struct kevent, 64> events;
    std::unordered_map<int, handler> handlers;
    int kq;
};
