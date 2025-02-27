#include "MenuState.hpp"

#include "Checkers.hpp"
#include "Engine.hpp"
#include "Game.hpp"
#include "Piece.hpp"
#include "ResourceManager.hpp"
#include "StateManager.hpp"

MenuState::MenuState(sf::RenderWindow& window, StateManager& stateManager, ResourceManager& resourceManager,
                     GameContext& gameContext) :
    State{
        window, stateManager, resourceManager, gameContext
},
    //// Select Checkers type
    selectInternationalTypeButton_{window, 250, resourceManager_.getFont(), "International", sf::Color{156, 62, 62}},
    selectRussianTypeButton_{window_, 320, resourceManager_.getFont(), "Russian", sf::Color{177, 143, 144}},
    selectCanadianTypeButton_{window_, 390, resourceManager_.getFont(), "Canadian", sf::Color{255, 21, 58}},
    selectBrazilianTypeButton_{window_, 460, resourceManager_.getFont(), "Brazilian", sf::Color{123, 206, 64}},
    //// Select Mode
    playWithComputerButton_{window, 280, resourceManager_.getFont(), "With Computer", sf::Color{34, 224, 216}},
    playTwoPlayersButton_{window_, 380, resourceManager_.getFont(), "Two Players", sf::Color{253, 0, 201}},
    //// Select Colour (if MODE == MODE::COMPUTER)
    playWhiteButton_{window, 280, resourceManager_.getFont(), "Play White", sf::Color{227, 154, 110}},
    playBlackButton_{window, 380, resourceManager_.getFont(), "Play Black", sf::Color{111, 128, 190}},
    //// Select EngineMode (if MODE == MODE::COMPUTER)
    selectEngineNoviceMode_{window, 240, resourceManager_.getFont(), "Novice", sf::Color{81, 148, 31}},
    selectEngineEasyMode_{window, 300, resourceManager_.getFont(), "Easy", sf::Color{207, 203, 88}},
    selectEngineMediumMode_{window, 360, resourceManager_.getFont(), "Medium", sf::Color{252, 82, 22}},
    selectEngineHardMode_{window, 420, resourceManager_.getFont(), "Hard", sf::Color{252, 149, 55}},
    selectEngineGrandmasterMode_{window, 480, resourceManager_.getFont(), "Grandmaster", sf::Color{255, 0, 118}}
{
    //// Select Checkers type
    selectInternationalTypeButton_.setCallback(
        [&checkersType = gameContext_.checkersType, &isCheckersTypeSelected = gameContext_.isCheckersTypeSelected]() {
            checkersType = CHECKERS_TYPE::INTERNATIONAL;
            isCheckersTypeSelected = true;
        });
    selectRussianTypeButton_.setCallback(
        [&checkersType = gameContext_.checkersType, &isCheckersTypeSelected = gameContext_.isCheckersTypeSelected]() {
            checkersType = CHECKERS_TYPE::RUSSIAN;
            isCheckersTypeSelected = true;
        });
    selectCanadianTypeButton_.setCallback(
        [&checkersType = gameContext_.checkersType, &isCheckersTypeSelected = gameContext_.isCheckersTypeSelected]() {
            checkersType = CHECKERS_TYPE::CANADIAN;
            isCheckersTypeSelected = true;
        });
    selectBrazilianTypeButton_.setCallback(
        [&checkersType = gameContext_.checkersType, &isCheckersTypeSelected = gameContext_.isCheckersTypeSelected]() {
            checkersType = CHECKERS_TYPE::BRAZILIAN;
            isCheckersTypeSelected = true;
        });

    // Select Mode
    playWithComputerButton_.setCallback([&mode = gameContext_.mode, &isModeSelected = gameContext_.isModeSelected]() {
        mode = MODE::COMPUTER;
        isModeSelected = true;
    });
    playTwoPlayersButton_.setCallback([&mode = gameContext_.mode, &isModeSelected = gameContext_.isModeSelected]() {
        mode = MODE::TWO_PLAYERS;
        isModeSelected = true;
    });

    // Select Colour
    playWhiteButton_.setCallback(
        [&playerColour = gameContext_.playerColour, &isColourSelected = gameContext_.isColourSelected]() {
            playerColour = COLOUR::WHITE;
            isColourSelected = true;
        });
    playBlackButton_.setCallback(
        [&playerColour = gameContext_.playerColour, &isColourSelected = gameContext_.isColourSelected]() {
            playerColour = COLOUR::BLACK;
            isColourSelected = true;
        });

    // Select EngineMode
    selectEngineNoviceMode_.setCallback(
        [&engineMode = gameContext_.engineMode, &isEngineModeSelected = gameContext_.isEngineModeSelected]() {
            engineMode = ENGINE_MODE::NOVICE;
            isEngineModeSelected = true;
        });
    selectEngineEasyMode_.setCallback(
        [&engineMode = gameContext_.engineMode, &isEngineModeSelected = gameContext_.isEngineModeSelected]() {
            engineMode = ENGINE_MODE::EASY;
            isEngineModeSelected = true;
        });
    selectEngineMediumMode_.setCallback(
        [&engineMode = gameContext_.engineMode, &isEngineModeSelected = gameContext_.isEngineModeSelected]() {
            engineMode = ENGINE_MODE::MEDIUM;
            isEngineModeSelected = true;
        });
    selectEngineHardMode_.setCallback(
        [&engineMode = gameContext_.engineMode, &isEngineModeSelected = gameContext_.isEngineModeSelected]() {
            engineMode = ENGINE_MODE::HARD;
            isEngineModeSelected = true;
        });
    selectEngineGrandmasterMode_.setCallback(
        [&engineMode = gameContext_.engineMode, &isEngineModeSelected = gameContext_.isEngineModeSelected]() {
            engineMode = ENGINE_MODE::GRANDMASTER;
            isEngineModeSelected = true;
        });
}

void MenuState::handleEvent(const sf::Event& event)
{
    if (!gameContext_.isCheckersTypeSelected) {
        selectInternationalTypeButton_.handleEvent(event);
        selectRussianTypeButton_.handleEvent(event);
        selectCanadianTypeButton_.handleEvent(event);
        selectBrazilianTypeButton_.handleEvent(event);
    } else {
        if (!gameContext_.isModeSelected) {
            playWithComputerButton_.handleEvent(event);
            playTwoPlayersButton_.handleEvent(event);
        } else {
            if (gameContext_.mode == MODE::COMPUTER) {
                if (!gameContext_.isColourSelected) {
                    playWhiteButton_.handleEvent(event);
                    playBlackButton_.handleEvent(event);
                } else {
                    if (!gameContext_.isEngineModeSelected) {
                        selectEngineNoviceMode_.handleEvent(event);
                        selectEngineEasyMode_.handleEvent(event);
                        selectEngineMediumMode_.handleEvent(event);
                        selectEngineHardMode_.handleEvent(event);
                        selectEngineGrandmasterMode_.handleEvent(event);
                    }
                }
            }
        }
    }
    if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::Escape) {
            stateManager_.setActiveState(STATE_TYPE::MenuState);
        }
    }
}

void MenuState::update()
{
    if (gameContext_.isModeSelected) {
        if (gameContext_.mode == MODE::COMPUTER) {
            if (gameContext_.isColourSelected && gameContext_.isEngineModeSelected) {
                stateManager_.setActiveState(STATE_TYPE::PlayState);
            }
        } else {  // MODE::TWO_PLAYERS
            stateManager_.setActiveState(STATE_TYPE::PlayState);
        }
    }
}

void MenuState::render()
{
    sf::Text text("Let's Play\n\n Checkers!!", resourceManager_.getFont(), 36);
    text.setFillColor(sf::Color::White);
    text.setPosition((600 - text.getGlobalBounds().width) / 2, 50);
    window_.draw(text);

    if (!gameContext_.isCheckersTypeSelected) {
        selectInternationalTypeButton_.draw();
        selectRussianTypeButton_.draw();
        selectCanadianTypeButton_.draw();
        selectBrazilianTypeButton_.draw();
    } else {
        if (!gameContext_.isModeSelected) {
            playWithComputerButton_.draw();
            playTwoPlayersButton_.draw();
        } else {
            if (gameContext_.mode == MODE::COMPUTER) {
                if (!gameContext_.isColourSelected) {
                    playWhiteButton_.draw();
                    playBlackButton_.draw();
                } else {
                    if (!gameContext_.isEngineModeSelected) {
                        selectEngineNoviceMode_.draw();
                        selectEngineEasyMode_.draw();
                        selectEngineMediumMode_.draw();
                        selectEngineHardMode_.draw();
                        selectEngineGrandmasterMode_.draw();
                    }
                }
            }
        }
    }
}

void MenuState::reset()
{
    gameContext_.reset();
}
