set(GUI_FILES
    main.cpp
    Game.cpp
    StateManager.cpp
    PlayState.cpp
)

add_executable(${PROJECT_NAME} ${GUI_FILES})

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/game)

target_link_libraries(${PROJECT_NAME} PRIVATE sfml-graphics sfml-window sfml-system checkers-game)
