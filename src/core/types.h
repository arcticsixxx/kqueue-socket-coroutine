#pragma once

#include <concepts>

#include <netinet/in.h>

struct socket_data {
    struct sockaddr_in addr;
    socklen_t addr_len;
    int fd;
};

struct endpoint {
    const char* host;
    uint16_t port;
};

template <typename T>
concept Buffer = requires(T a) {
    { a.size() } -> std::convertible_to<size_t>;
};
