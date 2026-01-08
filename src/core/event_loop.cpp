#include "event_loop.h"

#include "exceptions.h"
#include "kqueue_backend.h"

bool event_loop::add_handle(event event) {
    int fd = event.fd;
    handlers.push_back(std::move(event));

    return add_kevent(fd);
}

void event_loop::remove_handle(int fd) {
    std::erase_if(handlers, [fd](const auto& e) {
        return e.fd == fd; 
    });

    kq_backend.delete_event(fd);
}

void event_loop::loop() {
    while (true) {
        const auto events = kq_backend.get_ready_events();
        for (const auto& ev: events) {
            process_event(ev.ident);
        }
    }
}

void event_loop::process_event(int fd) {
    if (auto it = std::ranges::find(handlers, fd, &event::fd); it != handlers.end()) {
        if (it->type == event::handle_type::ACCEPT) {
            remove_handle(fd);
        }

        it->handle.resume();
    }
}