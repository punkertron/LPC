#include "StateManager.hpp"

#include <stdexcept>
#include <utility>

#include "State.hpp"

void StateManager::registerState(STATE_TYPE st, std::unique_ptr<State> state)
{
    states_.insert(std::make_pair(st, std::move(state)));
}

void StateManager::setActiveState(STATE_TYPE st)
{
    auto it = states_.find(st);
    if (it == states_.end()) {
        throw std::logic_error("Unknown type of State");
    }
    currentState_ = it->second.get();
}

void StateManager::handleEvent(const sf::Event& event)
{
    currentState_->handleEvent(event);
}

void StateManager::update()
{
    currentState_->update();
}

void StateManager::render()
{
    currentState_->render();
}
