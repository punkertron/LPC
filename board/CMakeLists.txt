set(SRC_FILES
    Board.cpp
)

add_library(checkers-board ${SRC_FILES})

target_include_directories(checkers-board PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
