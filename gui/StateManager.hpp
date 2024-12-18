#pragma once

#include <SFML/Graphics.hpp>
#include <memory>
#include <unordered_map>

#include "State.hpp"

enum class STATE_TYPE {
    PlayState
};

class StateManager final {
private:
    std::unordered_map<STATE_TYPE, std::unique_ptr<State> > states_;
    State* currentState_{nullptr};

public:
    StateManager() = default;

    void registerState(STATE_TYPE st, std::unique_ptr<State> state);
    void setActiveState(STATE_TYPE st);
    void handleEvent(const sf::Event& event);
    void update();
    void render();
};
