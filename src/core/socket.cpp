#include "socket.h"

#include <arpa/inet.h>

#include <unistd.h>

#include "exceptions.h"

namespace {
static constexpr const int max_conections = 64;
}

Socket::Socket(EventLoop &event_loop) noexcept : event_loop_(event_loop) {}

Socket::Socket(EventLoop& event_loop, const endpoint& ep)
    : event_loop_(event_loop) {
    openFileDescriptor();
    bind(ep);
}

Socket::~Socket() noexcept {
    close(socket_data_.fd);
}

void Socket::openFileDescriptor() {
    int res = socket(PF_INET, SOCK_STREAM, 0);
    if (res < 0) {
        throw file_descriptor_exception{res};
    }

    socket_data_.fd = res;
}

void Socket::setSocketData(const endpoint &ep) {
    std::memset(&socket_data_.addr, 0, sizeof(struct sockaddr_in));
    socket_data_.addr.sin_family = AF_INET;
    socket_data_.addr.sin_addr.s_addr = inet_addr(ep.host);
    socket_data_.addr.sin_port = htons(ep.port);

    socket_data_.addr_len = sizeof(socket_data_.addr);
}

void Socket::bind(const endpoint& ep) {
    setSocketData(ep);

    int res = ::bind(socket_data_.fd,
        reinterpret_cast<struct sockaddr*>(&socket_data_.addr),
        sizeof(socket_data_.addr));
    if (res < 0) {
        throw bind_addr_exception{res};
    }
}

void Socket::listen() {
    int res = ::listen(socket_data_.fd, max_conections);
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
