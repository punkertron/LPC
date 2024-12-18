#pragma once

#include "Board.hpp"
#include "State.hpp"

class PlayState final : public State {
private:
    Board b{};
    std::vector<std::vector<std::shared_ptr<Piece>>> copyBoard_;  // this is not actual board inside Board class

public:
    PlayState(sf::RenderWindow& window);
    void handleEvent(const sf::Event& event) override;
    void update() override;
    void render() override;
};
