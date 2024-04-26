add_library(${PROJECT_NAME}
            STATIC  acceptor.cc
                    buffer.cc
                    channel.cc
                    epoller.cc
                    event_loop.cc
                    inet_addr.cc
                    socket.cc
                    tcp_conn.cc
                    tcp_server.cc)

target_link_libraries(${PROJECT_NAME} PUBLIC compiler_flags)
target_include_directories(${PROJECT_NAME}
                            INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})