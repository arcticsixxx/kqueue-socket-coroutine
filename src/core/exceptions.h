#pragma once

#include <exception>
#include <string>


class base_server_exception : public std::exception {
public:
    base_server_exception(int error) : err_(error) {};
protected:
    int err_;
};


class file_descriptor_exception : public base_server_exception {
public:
    file_descriptor_exception(int err) : base_server_exception(err) {
        err_str += strerror(err);
    }

    const char* what() const noexcept override {
        return err_str.c_str();
    }

private:
    std::string err_str = "unable to open file descriptor\n";
};


class bind_addr_exception : public base_server_exception {
public:
    bind_addr_exception(int err) : base_server_exception(err) {
        err_str += strerror(err);
    }

    const char* what() const noexcept override {
        return err_str.c_str();
    }

private:
    std::string err_str = "unable to bind address\n";
};


class listen_exception : public base_server_exception {
public:
    listen_exception(int err) : base_server_exception(err) {
        err_str += strerror(err);
    }

    const char* what() const noexcept override {
        return err_str.c_str();
    }

private:
    std::string err_str = "unable to listen\n";
};


class accept_exception : public base_server_exception {
public:
    accept_exception(int err) : base_server_exception(err) {
        err_str += strerror(err);
    }

    const char* what() const noexcept override {
        return err_str.c_str();
    }

private:
    std::string err_str = "unable to accept:";
};

class read_exception : public base_server_exception {
public:
    read_exception(int err) : base_server_exception(err) {
        err_str += strerror(err);
    }

    const char* what() const noexcept override {
        return err_str.c_str();
    }

private:
    std::string err_str = "unable to read:";
};


class event_loop_exception : public std::exception {
public:
    const char* what() const noexcept override {
        return "kq err";
    }
};
