#pragma once

#include <coroutine>
#include <exception>

struct promise;
struct non_own_resumable : std::coroutine_handle<promise> {
    using promise_type = ::promise;
};

struct promise
{
    auto get_return_object() { return non_own_resumable{std::coroutine_handle<promise>::from_promise(*this)}; }
    std::suspend_never initial_suspend() noexcept { return {}; }
    std::suspend_never final_suspend() noexcept { return {}; }
    void return_void() {}
    void unhandled_exception() { std::rethrow_exception(std::move(std::current_exception())); }
};
