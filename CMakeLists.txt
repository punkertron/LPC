cmake_minimum_required(VERSION 3.25.1)
project(LPC LANGUAGES CXX)  # Let's Play Checkers!!

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
if(MSVC)
    add_compile_options(/W4 /permissive-)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Prefer SFML package config (Homebrew/vcpkg/etc.).
if(APPLE AND NOT DEFINED SFML_DIR)
    set(_sfml_prefix_candidates
        /opt/homebrew/opt/sfml
        /opt/homebrew/opt/sfml@3
        /usr/local/opt/sfml
        /usr/local/opt/sfml@3
    )

    foreach(_prefix IN LISTS _sfml_prefix_candidates)
        if(EXISTS "${_prefix}/lib/cmake/SFML/SFMLConfig.cmake")
            list(APPEND CMAKE_PREFIX_PATH "${_prefix}")
        endif()
    endforeach()
endif()

find_package(SFML 3 CONFIG REQUIRED COMPONENTS Graphics Window System)

add_subdirectory(checkers-logic)
add_subdirectory(engine)
add_subdirectory(gui)

add_executable(${PROJECT_NAME} main.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE gui)

set(LPC_RESOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/resources/queen.png
    ${CMAKE_CURRENT_SOURCE_DIR}/resources/you_lost.png
    ${CMAKE_CURRENT_SOURCE_DIR}/resources/you_win.png
    ${CMAKE_CURRENT_SOURCE_DIR}/resources/white_wins.png
    ${CMAKE_CURRENT_SOURCE_DIR}/resources/black_wins.png
    ${CMAKE_CURRENT_SOURCE_DIR}/resources/Sixtyfour-Regular.ttf
)

foreach(resource IN LISTS LPC_RESOURCES)
    add_custom_command(
        TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
                ${resource}
                $<TARGET_FILE_DIR:${PROJECT_NAME}>
    )
endforeach()
