#pragma once

#include <sys/event.h>
#include <unistd.h>

#include <vector>

#include "exceptions.h"

namespace {
static constexpr const size_t initial_events_size = 256;
}

class kqueue_backend {
public:
    kqueue_backend() {
        kq_ = kqueue();
        if (kq_ < 0) {
            throw kqueue_backend_exception{};
        }
    }

    ~kqueue_backend() {
        close(kq_);
    }

    inline int get_kq() const noexcept { return kq_; }

    inline bool add_kevent(int fd) noexcept {
        struct kevent kev;
        EV_SET(&kev, fd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, nullptr);
        return kevent(kq_, &kev, 1, nullptr, 0, nullptr) >= 0;
    }

    std::vector<struct kevent> get_ready_events() {
        std::vector<struct kevent> events(32);
        int n = kevent(kq_, nullptr, 0, events.data(), events.size(), nullptr);
        if (n <= 0) {
            return {};
            // throw;
        }

        events.resize(n);
        return events;
    }

    void delete_event(int fd) {
        struct kevent to_delete;
        EV_SET(&to_delete, fd, EVFILT_READ, EV_DELETE, 0, 0, nullptr);
        kevent(kq_, &to_delete, 1, nullptr, 0, nullptr);
    }

private:
    int kq_ = -1;
};