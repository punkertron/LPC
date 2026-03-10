#include "MenuState.hpp"

#include <algorithm>

#include "Checkers.hpp"
#include "Engine.hpp"
#include "Game.hpp"
#include "ResourceManager.hpp"
#include "StateManager.hpp"

namespace
{
constexpr float kBaseLayoutHeight = Game::MIN_WINDOW_HEIGHT;
constexpr float kTitleTopRatio = 50.f / kBaseLayoutHeight;
constexpr unsigned int kMinTitleCharacterSize = 36;
constexpr unsigned int kMaxTitleCharacterSize = 52;

constexpr float toRowRatio(float rowPosition)
{
    return rowPosition / kBaseLayoutHeight;
}
}  // namespace

MenuState::MenuState(sf::RenderWindow& window, StateManager& stateManager, ResourceManager& resourceManager,
                     GameContext& gameContext) :
    State{
        window, stateManager, resourceManager, gameContext
},
    //// Select Checkers type
    selectInternationalTypeButton_{window, toRowRatio(250.f), resourceManager_.getFont(), "International",
                                   sf::Color{156, 62, 62}},
    selectRussianTypeButton_{window_, toRowRatio(320.f), resourceManager_.getFont(), "Russian", sf::Color{177, 143, 144}},
    selectCanadianTypeButton_{window_, toRowRatio(390.f), resourceManager_.getFont(), "Canadian", sf::Color{255, 21, 58}},
    selectBrazilianTypeButton_{window_, toRowRatio(460.f), resourceManager_.getFont(), "Brazilian", sf::Color{123, 206, 64}},
    //// Select Mode
    playWithComputerButton_{window, toRowRatio(280.f), resourceManager_.getFont(), "With Computer", sf::Color{34, 224, 216}},
    playTwoPlayersButton_{window_, toRowRatio(380.f), resourceManager_.getFont(), "Two Players", sf::Color{253, 0, 201}},
    //// Select Colour (if MODE == MODE::COMPUTER)
    playWhiteButton_{window, toRowRatio(280.f), resourceManager_.getFont(), "Play White", sf::Color{227, 154, 110}},
    playBlackButton_{window, toRowRatio(380.f), resourceManager_.getFont(), "Play Black", sf::Color{111, 128, 190}},
    //// Select EngineMode (if MODE == MODE::COMPUTER)
    selectEngineNoviceMode_{window, toRowRatio(240.f), resourceManager_.getFont(), "Novice", sf::Color{81, 148, 31}},
    selectEngineEasyMode_{window, toRowRatio(300.f), resourceManager_.getFont(), "Easy", sf::Color{207, 203, 88}},
    selectEngineMediumMode_{window, toRowRatio(360.f), resourceManager_.getFont(), "Medium", sf::Color{252, 82, 22}},
    selectEngineHardMode_{window, toRowRatio(420.f), resourceManager_.getFont(), "Hard", sf::Color{252, 149, 55}},
    selectEngineGrandmasterMode_{window, toRowRatio(480.f), resourceManager_.getFont(), "Grandmaster",
                                 sf::Color{255, 0, 118}}
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
    if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>(); keyPressed != nullptr) {
        if (keyPressed->code == sf::Keyboard::Key::Escape) {
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
    const sf::Vector2u windowSize = window_.getSize();
    const auto referenceLength = static_cast<float>(std::min(windowSize.x, windowSize.y));
    const auto titleCharacterSize = static_cast<unsigned int>(std::clamp(
        referenceLength * 0.06f, static_cast<float>(kMinTitleCharacterSize), static_cast<float>(kMaxTitleCharacterSize)));

    sf::Text text{resourceManager_.getFont(), "Let's Play\n\n Checkers!!", titleCharacterSize};
    text.setFillColor(sf::Color::White);
    const auto titleBounds = text.getLocalBounds();
    text.setPosition({(static_cast<float>(windowSize.x) - titleBounds.size.x) / 2.f - titleBounds.position.x,
                      kTitleTopRatio * static_cast<float>(windowSize.y) - titleBounds.position.y});
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
