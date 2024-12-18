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
    static constexpr int WINDOW_HEIGHT{600};

    Game();

    void run();
};
