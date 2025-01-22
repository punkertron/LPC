set(SRC_FILES
    Board.cpp
    Piece.cpp
    Checkers.cpp
)

add_library(checkers-logic ${SRC_FILES})

target_include_directories(checkers-logic PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
