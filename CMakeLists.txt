cmake_minimum_required(VERSION 3.7)
project(Client-Server)

set(CMAKE_C_STANDARD 99)

add_executable(Client Client/Client.c)
add_executable(Server Server/Server.c)
