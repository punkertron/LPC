#pragma once

#include <SFML/Graphics.hpp>

#include "Button.hpp"
#include "State.hpp"

class ResourceManager;
class StateManager;
struct GameContext;

class MenuState final : public State {
private:
    Button selectInternationalTypeButton_;
    Button selectRussianTypeButton_;
    Button selectCanadianTypeButton_;
    Button selectBrazilianTypeButton_;
    Button playWithComputerButton_;
    Button playTwoPlayersButton_;
    Button playWhiteButton_;
    Button playBlackButton_;
    Button selectEngineNoviceMode_;
    Button selectEngineEasyMode_;
    Button selectEngineMediumMode_;
    Button selectEngineHardMode_;
    Button selectEngineGrandmasterMode_;

public:
    MenuState(sf::RenderWindow& window, StateManager& stateManager, ResourceManager& resourceManager,
              GameContext& gameContext);

    void handleEvent(const sf::Event& event) override;
    void update() override;
    void render() override;
    void reset() override;
};
