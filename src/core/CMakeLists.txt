add_library(
    core STATIC
    socket.h socket.cpp
    exceptions.h
    coroutine_ops.h
    types.h
    event_loop.h
    event_loop.cpp
)

target_include_directories(
  core ${warning_guard}
  PUBLIC
  "\$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src/core>"
)

