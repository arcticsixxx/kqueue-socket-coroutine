#include "event_loop.h"

#include "exceptions.h"

#include <unistd.h>

namespace {
static constexpr const size_t initial_events_size = 256;
}

EventLoop::EventLoop() {
    kq = kqueue();
    if (kq < 0) {
        throw event_loop_exception{};
    }

    events.resize(initial_events_size);
}

EventLoop::~EventLoop() noexcept {
    close(kq);
}

void EventLoop::loop() {
    while (true) {
        int n = kevent(kq, nullptr, 0, events.data(), events.size(), nullptr);
        for (int i = 0; i < n; ++i) {
            int fd = events[i].ident;
            if (handlers.contains(fd)) {
                auto h = handlers[fd];
                if (h.type == handle_type::ACCEPT) {
                    handlers.erase(fd);
                }

                h.handle.resume();
            }
        }
    }
}
