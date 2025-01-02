#pragma once

#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>

#include "Board.hpp"
#include "Engine.hpp"
#include "Move.hpp"
#include "Piece.hpp"
#include "State.hpp"

class ResourceManager;
class StateManager;
struct GameContext;

class PlayState final : public State {
private:
    Board board_{};
    std::vector<std::vector<std::shared_ptr<Piece>>> copyBoard_;  // this is not actual board inside Board class
    Position lastSelectedPosition_;
    Position makeMoveTo_;
    bool isSquareSelected_;
    bool isMoveInProcess_;
    bool isUniqueWayToNewPosition_;
    bool isGameOver_;
    float movePosition_;
    Move oneWayMove_;
    std::vector<Move> possibleMoves_;

    COLOUR currentMoveColor_;
    COLOUR winnerColor_;
    std::unique_ptr<Engine> engine_;
    sf::Clock clock;

    Position getPositionOnBoardFromMouse(int x, int y);
    void drawBoard();
    void highlightPossibleMoves(Position pos);
    bool isPositionInsidePossibleMoves(Position pos);
    bool isOneWayTo(Position to);
    void renderResult();
    void processMove();
    void drawPiece(Position pos, bool isMoving = false);

public:
    PlayState(sf::RenderWindow& window, StateManager& stateManager, ResourceManager& resourceManager,
              GameContext& gameContext);
    void handleEvent(const sf::Event& event) override;
    void update() override;
    void render() override;
    void reset() override;
};
