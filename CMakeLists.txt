cmake_minimum_required(VERSION 3.25.1)
project(CheckersGame LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(SOURCE_DIR game)

set(SOURCES
    ${SOURCE_DIR}/main.cpp
    ${SOURCE_DIR}/Board.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE ${SOURCE_DIR})