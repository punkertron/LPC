#include "Game.hpp"

#include "PlayState.hpp"

Game::Game() : window_{sf::VideoMode(WINDOW_WIDTH, WINDOW_WIDTH), "Checkers Client!", sf::Style::Titlebar | sf::Style::Close}
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

    // TODO: add menu
    stateManager_.registerState(STATE_TYPE::PlayState, std::make_unique<PlayState>(window_, resourceManager_));
    stateManager_.setActiveState(STATE_TYPE::PlayState);
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
