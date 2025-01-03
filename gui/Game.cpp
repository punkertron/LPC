#include "Game.hpp"

#include <memory>

#include "MenuState.hpp"
#include "PlayState.hpp"
#include "State.hpp"

Game::Game() :
    window_{sf::VideoMode(WINDOW_WIDTH, WINDOW_WIDTH), "LPD - Let's Play Checkers!!", sf::Style::Titlebar | sf::Style::Close}
{
    // Calculate the position to the center of Desktop
    sf::VideoMode desktopMode = sf::VideoMode::getDesktopMode();
    auto screenWidth = desktopMode.width;
    auto screenHeight = desktopMode.height;
    auto windowPosX = (screenWidth - WINDOW_WIDTH) / 2;
    auto windowPosY = (screenHeight - WINDOW_WIDTH) / 2;
    if (screenWidth > WINDOW_WIDTH && screenHeight > WINDOW_WIDTH) {
        // strange if not true, but who knows
        window_.setPosition(sf::Vector2i(windowPosX, windowPosY));
    }

    window_.setFramerateLimit(50);

    stateManager_.registerState(STATE_TYPE::MenuState,
                                std::make_unique<MenuState>(window_, stateManager_, resourceManager_, gameContext_));
    stateManager_.registerState(STATE_TYPE::PlayState,
                                std::make_unique<PlayState>(window_, stateManager_, resourceManager_, gameContext_));
    stateManager_.setActiveState(STATE_TYPE::MenuState);
}

void Game::run()
{
    while (window_.isOpen()) {
        processEvents();
        render();
        update();
    }
}

void Game::processEvents()
{
    sf::Event event;
    while (window_.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window_.close();
        }
        stateManager_.handleEvent(event);
    }
}

void Game::update()
{
    stateManager_.update();
}

void Game::render()
{
    window_.clear();
    stateManager_.render();
    window_.display();
}
