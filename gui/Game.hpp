#pragma once

#include <SFML/Graphics.hpp>

#include "Checkers.hpp"
#include "Engine.hpp"
#include "ResourceManager.hpp"
#include "StateManager.hpp"

enum class MODE {
    COMPUTER,
    TWO_PLAYERS,
};

struct GameContext {
    CHECKERS_TYPE checkersType{CHECKERS_TYPE::RUSSIAN};
    MODE mode{MODE::COMPUTER};
    COLOUR playerColour{COLOUR::WHITE};
    ENGINE_MODE engineMode{ENGINE_MODE::GRANDMASTER};
    bool isCheckersTypeSelected{false};
    bool isModeSelected{false};
    bool isColourSelected{false};
    bool isEngineModeSelected{false};

    void reset()
    {
        isCheckersTypeSelected = false;
        isModeSelected = false;
        isColourSelected = false;
        isEngineModeSelected = false;
    }
};

class Game final {
private:
    sf::RenderWindow window_;
    StateManager stateManager_;
    ResourceManager resourceManager_;
    GameContext gameContext_;

    void processEvents();
    void update();
    void render();

public:
    static constexpr unsigned int MIN_WINDOW_WIDTH{600};
    static constexpr unsigned int MIN_WINDOW_HEIGHT{600};

    Game();

    void run();
};
