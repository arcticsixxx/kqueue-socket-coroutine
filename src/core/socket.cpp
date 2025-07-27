#include "socket.h"

#include <arpa/inet.h>

#include <unistd.h>

#include "exceptions.h"

Socket::Socket(event_loop &event_loop) noexcept
    : event_loop_(event_loop) {}

Socket::Socket(event_loop &event_loop, socket_data &&socket_data)
    : event_loop_(event_loop)
    , socket_data_(socket_data) {}

Socket::Socket(event_loop& event_loop, const endpoint& ep)
    : event_loop_(event_loop) {
    open_file_descriptor();
    bind(ep);
}

Socket::~Socket() noexcept {
    if (socket_data_.fd >= 0) {
        close(socket_data_.fd);
    }
}

void Socket::open_file_descriptor() {
    int res = socket(PF_INET, SOCK_STREAM, 0);
    if (res < 0) {
        throw file_descriptor_exception{res};
    }

    socket_data_.fd = res;
}

void Socket::set_socket_data(const endpoint &ep) {
    std::memset(&socket_data_.addr, 0, sizeof(struct sockaddr_in));
    socket_data_.addr.sin_family = AF_INET;
    socket_data_.addr.sin_addr.s_addr = inet_addr(ep.host);
    socket_data_.addr.sin_port = htons(ep.port);

    socket_data_.addr_len = sizeof(socket_data_.addr);
}

void Socket::bind(const endpoint& ep) {
    set_socket_data(ep);

    int res = ::bind(socket_data_.fd,
        reinterpret_cast<struct sockaddr*>(&socket_data_.addr),
        sizeof(socket_data_.addr));
    if (res < 0) {
        throw bind_addr_exception{res};
    }
}

void Socket::listen(size_t max_connections) {
    int res = ::listen(socket_data_.fd, max_connections);
    if (res < 0) {
        throw listen_exception{res};
    }
}

socket_data Socket::accept() {
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    int client_fd = ::accept(socket_data_.fd,
        reinterpret_cast<struct sockaddr*>(&client_addr),
        &client_len);

    if (client_fd < 0) {
        throw accept_exception{client_fd};
    }

    return { client_addr, client_len, client_fd };
}
