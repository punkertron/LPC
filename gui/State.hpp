#pragma once

#include <SFML/Graphics.hpp>

class State {
protected:
    sf::RenderWindow& window_;

public:
    State(sf::RenderWindow& window) : window_{window}
    {
    }

    virtual ~State() = default;
    virtual void handleEvent(const sf::Event& event) = 0;
    virtual void update() = 0;
    virtual void render() = 0;
};
