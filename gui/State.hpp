#pragma once

#include <SFML/Graphics.hpp>

#include "ResourceManager.hpp"

class State {
protected:
    sf::RenderWindow& window_;
    ResourceManager& resourceManager_;

public:
    State(sf::RenderWindow& window, ResourceManager& resourceManager) : window_{window}, resourceManager_{resourceManager}
    {
    }

    virtual ~State() = default;
    virtual void handleEvent(const sf::Event& event) = 0;
    virtual void update() = 0;
    virtual void render() = 0;
};
