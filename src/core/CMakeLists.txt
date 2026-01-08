add_library(
    core STATIC
    socket.h socket.cpp
    exceptions.h
    non_own_resumable.h
    types.h
    event_loop.h
    event_loop.cpp
    accept_awaitable.h
    read_awaitable.h
)

target_include_directories(
  core ${warning_guard}
  PUBLIC
  "\$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src/core>"
)

