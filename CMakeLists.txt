cmake_minimum_required(VERSION 3.25.1)
project(LPC LANGUAGES CXX)  # Let's Play Checkers!!

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
add_compile_options(-Wall -Wextra -Wpedantic)

find_package(SFML 2.5 COMPONENTS graphics window system REQUIRED)

add_subdirectory(checkers-logic)
add_subdirectory(engine)
add_subdirectory(gui)

add_executable(${PROJECT_NAME} main.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE gui)

file(COPY
        ${CMAKE_CURRENT_SOURCE_DIR}/resources/queen.png
        ${CMAKE_CURRENT_SOURCE_DIR}/resources/you_lost.png
        ${CMAKE_CURRENT_SOURCE_DIR}/resources/you_win.png
        ${CMAKE_CURRENT_SOURCE_DIR}/resources/white_wins.png
        ${CMAKE_CURRENT_SOURCE_DIR}/resources/black_wins.png
        ${CMAKE_CURRENT_SOURCE_DIR}/resources/Sixtyfour-Regular.ttf
    DESTINATION ${CMAKE_CURRENT_BINARY_DIR}
)
