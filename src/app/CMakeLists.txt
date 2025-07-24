add_executable(example_app main.cpp)

target_link_libraries(example_app PRIVATE
    core
)

include(GNUInstallDirs)
install(TARGETS example_app
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
