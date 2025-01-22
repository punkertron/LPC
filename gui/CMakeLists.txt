set(SRC_FILES
    Game.cpp
    StateManager.cpp
    ResourceManager.cpp
    Button.cpp
    MenuState.cpp
    PlayState.cpp
)

add_library(gui ${SRC_FILES})

target_include_directories(gui PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(gui PRIVATE sfml-graphics sfml-window sfml-system)
target_link_libraries(gui PUBLIC checkers-logic checkers-engine)
