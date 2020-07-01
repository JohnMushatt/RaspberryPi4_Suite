add_library(server_lib  rpi_server.c rpi_server.h thread_pool.c thread_pool.h)
target_include_directories(server_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

