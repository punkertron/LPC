#pragma once

#include <string_view>

#include "Button.hpp"
#include "State.hpp"

class MenuState final : public State {
private:
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
