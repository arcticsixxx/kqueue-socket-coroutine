#include "event_loop.h"

#include "exceptions.h"

#include <unistd.h>

namespace {
static constexpr const size_t initial_events_size = 256;
}

event_loop::event_loop() {
    kq = kqueue();
    if (kq < 0) {
        throw event_loop_exception{};
    }

    events.resize(initial_events_size);
}

event_loop::~event_loop() noexcept {
    if (kq >= 0) {
        close(kq);
    }
}

void event_loop::loop() {
    while (true) {
        int n = kevent(kq, nullptr, 0, events.data(), events.size(), nullptr);
        for (int i = 0; i < n; ++i) {
            int fd = events[i].ident;

            if (auto it = std::ranges::find(handlers, fd, &event::fd);
                it != handlers.end()) {

                auto h = *it;
                if (h.type == handle_type::ACCEPT) {
                    remove_handle(fd);
                }

                h.handle.resume();
            }
        }
    }
}
