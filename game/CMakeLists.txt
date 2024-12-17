set(SRC_FILES
    Board.cpp
)

add_library(checkers-game ${SRC_FILES})

target_include_directories(checkers-game PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
