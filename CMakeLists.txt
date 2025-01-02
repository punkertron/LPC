cmake_minimum_required(VERSION 3.25.1)
project(CheckersGame LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(SFML 2.5 COMPONENTS graphics window system REQUIRED)

add_subdirectory(board)
add_subdirectory(engine)
add_subdirectory(gui)
