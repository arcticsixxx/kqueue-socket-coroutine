#pragma once

#include <cstddef>
#include <sys/event.h>

#include <coroutine>
#include <vector>

#include "kqueue_backend.h"

struct event {
    enum handle_type {
        ACCEPT = 0,
        READ,
        WRITE
    };

    std::coroutine_handle<> handle;
    handle_type type;
    int fd;
};

class event_loop {
public:
    explicit event_loop() = default;
    event_loop(const event_loop&) = delete;
    auto operator=(const event_loop&) -> event_loop& = delete;
    event_loop(event_loop&&) = delete;
    auto operator=(event_loop&&) -> event_loop& = delete;
    ~event_loop() = default;

    void run() {
        // is_running = true;
        loop();
    }

    void stop() {
        // is_running = false;
    };

    int get_kq() const noexcept {
        return kq_backend.get_kq();
    }

    bool add_kevent(int fd) noexcept {
        return kq_backend.add_kevent(fd);
    }

    bool add_handle(event event);
    void remove_handle(int fd);

private:
    void loop();
    void process_event(int fd);

private:
    std::vector<event> handlers;
    kqueue_backend kq_backend;

    // std::atomic_bool is_running = false;
};
