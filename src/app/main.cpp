#include <deque>
#include <iostream>
#include <list>
#include <span>

#include "socket.h"

struct server {
    server(EventLoop& event_loop, const endpoint& ep)
        : event_loop_{event_loop}
        , s{event_loop_, ep} {}

    void run() {
        loop();
    }

private:
    coro_task loop() {
        s.listen();

        for (;;) {
            auto client = co_await s.asyncAccept();
            handle_client(client);
        }
    }

    template<typename T>
    void printData(T& t) {
        for (const auto& el : t) {
            std::cout << el;
        }

        std::cout << std::endl;
    }

    coro_task handle_client(socket_data client) {
        for (;;) {
            std::array<char, 4096> data{};
            auto read = co_await s.asyncRead(client.fd, data);
            if (!read) {
                break;
            }
            std::cout << client.fd << " " << read << ": " << data.data() << std::endl;
        }

        std::cout << client.fd << " disconnected" << std::endl;
    }

    EventLoop& event_loop_;
    Socket s;
};

int main() {
    EventLoop loop;
    const endpoint ep = {
        .host = "127.0.0.1",
        .port = 3336u
    };

    std::unique_ptr<server> s;

    try {
        s = std::make_unique<server>(loop, ep);
    } catch(const std::exception& ex) {
        std::cerr << ex.what() << "\n";
    }

    s->run();
    loop.run();

    return 0;
}
