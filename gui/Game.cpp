#include "Game.hpp"

#include <memory>
#include <optional>

#include "MenuState.hpp"
#include "PlayState.hpp"
#include "State.hpp"

Game::Game() : window_{sf::VideoMode({MIN_WINDOW_WIDTH, MIN_WINDOW_HEIGHT}), "LPC - Let's Play Checkers!!"}
{
    // Calculate the position to the center of Desktop
    const sf::VideoMode desktopMode = sf::VideoMode::getDesktopMode();
    const auto screenWidth = desktopMode.size.x;
    const auto screenHeight = desktopMode.size.y;
    const auto windowPosX = (screenWidth - MIN_WINDOW_WIDTH) / 2;
    const auto windowPosY = (screenHeight - MIN_WINDOW_HEIGHT) / 2;
    if (screenWidth > MIN_WINDOW_WIDTH && screenHeight > MIN_WINDOW_HEIGHT) {
        // strange if not true, but who knows
        window_.setPosition(sf::Vector2i(static_cast<int>(windowPosX), static_cast<int>(windowPosY)));
    }

    window_.setMinimumSize(sf::Vector2u{MIN_WINDOW_WIDTH, MIN_WINDOW_HEIGHT});
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
    while (const auto event = window_.pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            window_.close();
        }
        if (const auto* resized = event->getIf<sf::Event::Resized>();
            resized != nullptr && resized->size.x > 0 && resized->size.y > 0) {
            window_.setView(sf::View(
                sf::FloatRect({0.f, 0.f}, {static_cast<float>(resized->size.x), static_cast<float>(resized->size.y)})));
        }
        stateManager_.handleEvent(*event);
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
