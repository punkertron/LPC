set(SRC_FILES
    EvaluationFunction.cpp    
    MinimaxEngine.cpp
    RandomEngine.cpp
)

add_library(checkers-engine ${SRC_FILES})

target_include_directories(checkers-engine PRIVATE ${CMAKE_SOURCE_DIR}/game)
target_include_directories(checkers-engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
