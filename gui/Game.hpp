#pragma once

#include <SFML/Graphics.hpp>

#include "StateManager.hpp"

class Game final {
private:
    sf::RenderWindow window_;
    StateManager stateManager_;

    void processEvents();
    void update();
    void render();

public:
    static constexpr int WINDOW_WIDTH{600};
    static constexpr int BOARD_SIZE{8};  // ???

    Game();

    void run();
};
