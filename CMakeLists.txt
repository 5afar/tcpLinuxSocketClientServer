cmake_minimum_required(VERSION 3.28)
project(TCPClientServer)

set(CMAKE_CXX_STANDARD 17)

add_executable(server server/src/server.cpp)
add_executable(client client/src/client.cpp)