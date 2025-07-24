#include "event_loop.h"

#include "exceptions.h"

#include <unistd.h>

EventLoop::EventLoop() {
    kq = kqueue();
    if (kq < 0) {
        throw event_loop_exception{};
    }
}

EventLoop::~EventLoop() noexcept {
    close(kq);
}

void EventLoop::loop() {
    while (true) {
        int n = kevent(kq, nullptr, 0, events.data(), 64, nullptr);
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
