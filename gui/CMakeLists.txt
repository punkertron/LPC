set(GUI_FILES
    main.cpp
    Game.cpp
    StateManager.cpp
    ResourceManager.cpp
    PlayState.cpp
)

add_executable(${PROJECT_NAME} ${GUI_FILES})

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/game ${CMAKE_SOURCE_DIR}/engine)

target_link_libraries(${PROJECT_NAME} PRIVATE sfml-graphics sfml-window sfml-system checkers-game checkers-engine)

file(COPY
        ${CMAKE_CURRENT_SOURCE_DIR}/resources/queen.png
        ${CMAKE_CURRENT_SOURCE_DIR}/resources/you_lost.png
        ${CMAKE_CURRENT_SOURCE_DIR}/resources/you_win.png
        ${CMAKE_CURRENT_SOURCE_DIR}/resources/white_wins.png
        ${CMAKE_CURRENT_SOURCE_DIR}/resources/black_wins.png
        ${CMAKE_CURRENT_SOURCE_DIR}/resources/Sixtyfour-Regular.ttf
    DESTINATION ${CMAKE_CURRENT_BINARY_DIR}
)
