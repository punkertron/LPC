#pragma once

#include <SFML/Graphics.hpp>

class StateManager;
class ResourceManager;
struct GameContext;

class State {
protected:
    sf::RenderWindow& window_;
    StateManager& stateManager_;
    ResourceManager& resourceManager_;
    GameContext& gameContext_;

public:
    State(sf::RenderWindow& window, StateManager& stateManager, ResourceManager& resourceManager, GameContext& gameContext) :
        window_{window}, stateManager_{stateManager}, resourceManager_{resourceManager}, gameContext_{gameContext}
    {
    }

    virtual ~State() = default;
    virtual void handleEvent(const sf::Event& event) = 0;
    virtual void update() = 0;
    virtual void render() = 0;
    virtual void reset() = 0;
};
