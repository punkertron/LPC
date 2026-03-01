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

if(TARGET SFML::Graphics)
    target_link_libraries(gui PUBLIC SFML::Graphics SFML::Window SFML::System)
elseif(TARGET sfml-graphics)
    target_link_libraries(gui PUBLIC sfml-graphics sfml-window sfml-system)
else()
    message(FATAL_ERROR "SFML targets were not found. Check SFML installation and CMake configuration.")
endif()

target_link_libraries(gui PUBLIC checkers-logic checkers-engine)
