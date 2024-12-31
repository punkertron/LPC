#pragma once

#include "Board.hpp"
#include "Engine.hpp"
#include "MinimaxEngine.hpp"
#include "RandomEngine.hpp"
#include "ResourceManager.hpp"
#include "State.hpp"

class PlayState final : public State {
private:
    Board board_{};
    std::vector<std::vector<std::shared_ptr<Piece>>> copyBoard_;  // this is not actual board inside Board class
    Position lastSelectedPosition_;
    Position makeMoveTo_;
    bool isSquareSelected_{false};
    bool isMoveInProcess_{false};
    bool isUniqueWayToNewPosition_{false};
    float movePosition_{0.f};
    Move oneWayMove_;
    std::vector<Move> possibleMoves_;

    // engine related
    COLOUR playerColor_{COLOUR::WHITE};
    std::unique_ptr<Engine> engine_ = std::make_unique<MinimaxEngine>(board_, ENGINE_MODE::EASY);

    Position getPositionOnBoardFromMouse(int x, int y);
    void drawBoard();
    void highlightPossibleMoves(Position pos);
    bool isPositionInsidePossibleMoves(Position pos);
    bool isOneWayTo(Position to);
    void processMove();
    void drawPiece(Position pos, bool isMoving = false);

public:
    PlayState(sf::RenderWindow& window, ResourceManager& resourceManager);
    void handleEvent(const sf::Event& event) override;
    void update() override;
    void render() override;
};
