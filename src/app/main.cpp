#include <iostream>

#include "socket.h"

struct server {
    server(event_loop& event_loop, const endpoint& ep)
        : event_loop_{event_loop}
        , ep{ep} {
    }

    void run() {
        loop();
    }

private:
    auto loop() -> coro_task {
        Socket s{event_loop_, ep};
        s.listen();

        for (;;) {
            auto client = co_await s.async_accept();
            handle_client({event_loop_, std::move(client)});
        }

        co_return;
    }

    auto handle_client(Socket client) -> coro_task {
        for (;;) {
            std::array<char, 256> data{};
            auto read = co_await client.async_read(client.get_fd(), data);
            if (read == 0) {
                break;
            }
            std::cout << client.get_fd() << " " << read << ": " << data.data() << std::endl;
        }

        std::cout << client.get_fd() << " disconnected" << std::endl;
        co_return;
    }

    event_loop& event_loop_;
    const endpoint ep;
};

int main() {
    event_loop loop;
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
