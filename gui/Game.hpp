#pragma once

#include <SFML/Graphics.hpp>

#include "Checkers.hpp"
#include "ResourceManager.hpp"
#include "StateManager.hpp"

enum class MODE {
    COMPUTER,
    TWO_PLAYERS,
};

enum class ENGINE_MODE;

struct GameContext {
    CHECKERS_TYPE checkersType;
    MODE mode;
    COLOUR playerColour;
    ENGINE_MODE engineMode;
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
    static constexpr int WINDOW_WIDTH{600};

    Game();

    void run();
};
