cmake_minimum_required(VERSION 3.25.1)
project(LPC LANGUAGES CXX)  # Let's Play Checkers!!

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
add_compile_options(-Wall -Wextra -Wpedantic)

# Prefer SFML package config (Homebrew/vcpkg/etc.) and fall back to CMake's FindSFML module.
if(APPLE AND NOT DEFINED SFML_DIR)
    set(_sfml_prefix_candidates
        /opt/homebrew/opt/sfml
        /opt/homebrew/opt/sfml@2
        /usr/local/opt/sfml
        /usr/local/opt/sfml@2
    )

    foreach(_prefix IN LISTS _sfml_prefix_candidates)
        if(EXISTS "${_prefix}/lib/cmake/SFML/SFMLConfig.cmake")
            list(APPEND CMAKE_PREFIX_PATH "${_prefix}")
        endif()
    endforeach()
endif()

find_package(SFML 2.5 CONFIG QUIET COMPONENTS Graphics Window System)
if(NOT SFML_FOUND)
    find_package(SFML 2.5 REQUIRED COMPONENTS graphics window system)
endif()

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
