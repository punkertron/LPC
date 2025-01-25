#include "PlayState.hpp"

#include <algorithm>
#include <cassert>
#include <utility>

#include "Game.hpp"
#include "MinimaxEngine.hpp"
#include "Piece.hpp"
#include "RandomEngine.hpp"
#include "ResourceManager.hpp"
#include "StateManager.hpp"

PlayState::PlayState(sf::RenderWindow& window, StateManager& stateManager, ResourceManager& resourceManager,
                     GameContext& gameContext) :
    State{window, stateManager, resourceManager, gameContext}
{
    highlightCircle_.setFillColor(sf::Color{0, 255, 0, 150});  // Semi-transparent green

    whitePieceCircle_.setFillColor(sf::Color{240, 240, 210});
    whitePieceCircle_.setOutlineThickness(3);
    whitePieceCircle_.setOutlineColor(sf::Color{150, 150, 150});  // grey

    blackPieceCircle_.setFillColor(sf::Color{75, 75, 75});
    blackPieceCircle_.setOutlineThickness(3);
    blackPieceCircle_.setOutlineColor(sf::Color{150, 150, 150});  // grey

    whiteSquare_.setFillColor(sf::Color(240, 217, 181));

    blackSquare_.setFillColor(sf::Color{181, 136, 99});
}

Position PlayState::getPositionOnBoardFromMouse(int x, int y)
{
    int row = y / tile_;
    int col = x / tile_;
    return {row, col};
}

bool PlayState::isPositionInsidePossibleMoves(Position pos)
{
    for (const auto& m : possibleMoves_) {
        const Move* move = &m;
        do {
            if (move->to == pos) {
                return true;
            }
            move = move->nextMove.get();
        } while (move);
    }
    return false;
}

bool PlayState::isOneWayTo(Position to)
{
    int cnt = 0;
    for (const auto& m : possibleMoves_) {
        const Move* move = &m;
        do {
            if (move->to == to) {
                oneWayMove_ = m;
                ++cnt;
            }
            move = move->nextMove.get();
        } while (move);
    }

    return cnt == 1;
}

void PlayState::handleEvent(const sf::Event& event)
{
    if (!isGameOver_) {
        if ((gameContext_.mode == MODE::TWO_PLAYERS || checkers_.getCurrentColour() == gameContext_.playerColour) &&
            (!isMoveInProcess_ || (isMoveInProcess_ && !isUniqueWayToNewPosition_))) {
            if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    int x = event.mouseButton.x;
                    int y = event.mouseButton.y;

                    if (!isSquareSelected_) {
                        lastSelectedPosition_ = getPositionOnBoardFromMouse(x, y);
                        possibleMoves_ = checkers_.getValidMoves(lastSelectedPosition_);
                        if (possibleMoves_.empty()) {
                            return;
                        }
                        isSquareSelected_ = true;
                    } else {
                        auto pos = getPositionOnBoardFromMouse(x, y);
                        if (pos == lastSelectedPosition_) {
                            isSquareSelected_ = false;
                            possibleMoves_.clear();
                        } else if (isPositionInsidePossibleMoves(pos)) {
                            isMoveInProcess_ = true;
                            makeMoveTo_ = pos;
                            isUniqueWayToNewPosition_ = isOneWayTo(pos);
                        } else {
                            lastSelectedPosition_ = getPositionOnBoardFromMouse(x, y);
                            possibleMoves_ = checkers_.getValidMoves(lastSelectedPosition_);
                        }
                    }
                }
            }
        }
    }
    if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::Escape) {
            stateManager_.setActiveState(STATE_TYPE::MenuState);
        }
    }
}

void PlayState::update()
{
    if (!isGameOver_) {
        if (auto result = checkers_.getResult(); result.isOver) {
            isGameOver_ = true;
            winnerColor_ = result.winner;
            clock.restart();
        }
    }
    if (!isGameOver_) {
        if (gameContext_.mode == MODE::COMPUTER && checkers_.getCurrentColour() != gameContext_.playerColour &&
            !isMoveInProcess_) {
            oneWayMove_ = engine_->getBestMove();
            isMoveInProcess_ = true;
            isUniqueWayToNewPosition_ = true;
            lastSelectedPosition_ = oneWayMove_.from;
            Move* ptr = &oneWayMove_;
            while (ptr->nextMove) {
                ptr = ptr->nextMove.get();
            }
            makeMoveTo_ = ptr->to;
        }
    }
}

void PlayState::drawBoard()
{
    for (int row = 0; row < copyBoard_.getWidth(); ++row) {
        for (int col = 0; col < copyBoard_.getWidth(); ++col) {
            sf::RectangleShape& square = ((row + col) % 2 == 0 ? whiteSquare_ : blackSquare_);
            square.setPosition(col * tile_, row * tile_);
            window_.draw(square);
        }
    }
}

void PlayState::renderResult()
{
    sf::RectangleShape greyOverlay(sf::Vector2f{Game::WINDOW_WIDTH, Game::WINDOW_WIDTH});
    greyOverlay.setFillColor(sf::Color{128, 128, 128, 190});  // Semi-transparent green
    window_.draw(greyOverlay);

    sf::Sprite sprite;
    if (gameContext_.mode == MODE::COMPUTER) {
        sprite.setTexture((winnerColor_ == gameContext_.playerColour ? resourceManager_.getYouWinTexture()
                                                                     : resourceManager_.getYouLostTexture()));
    } else {
        sprite.setTexture(resourceManager_.getColourWinsTexture(winnerColor_));
    }
    sf::Vector2f currentPosSprite = sf::Vector2f{(Game::WINDOW_WIDTH - sprite.getGlobalBounds().width) / 2, 150};

    sprite.setPosition(currentPosSprite);
    window_.draw(sprite);

    if (clock.getElapsedTime().asSeconds() > 2) {
        sf::Text message{"Press Esc to return to the menu", resourceManager_.getFont(), 16};
        message.setFillColor(sf::Color::White);
        message.setPosition((600 - message.getGlobalBounds().width) / 2,
                            sprite.getGlobalBounds().height + sprite.getPosition().y + 20);
        window_.draw(message);
    }
}

void PlayState::render()
{
    drawBoard();

    // draw pieces
    for (int row = 0; row < copyBoard_.getWidth(); ++row) {
        for (int col = (row % 2 ? 0 : 1); col < copyBoard_.getWidth(); col += 2) {
            if (auto piece = copyBoard_(row, col); piece.isNotEmpty()) {
                if (!isMoveInProcess_ || !(lastSelectedPosition_ == Position{row, col})) {
                    drawPiece(Position{row, col});
                }
            }

            if ((!isMoveInProcess_ || !isUniqueWayToNewPosition_) && !possibleMoves_.empty()) {
                highlightPossibleMoves({row, col});
            }
        }
    }

    if (isGameOver_) {
        renderResult();
    } else if (isMoveInProcess_) {
        processMove();
    }
}

void PlayState::processMove()
{
    if (isMoveInProcess_) {
        drawPiece(lastSelectedPosition_, true);
    }

    if (movePosition_ >= 1.f) {
        movePosition_ = 0;
        if (isUniqueWayToNewPosition_) {
            isMoveInProcess_ = false;
        } else {
            std::swap(copyBoard_(makeMoveTo_), copyBoard_(lastSelectedPosition_));
            lastSelectedPosition_ = makeMoveTo_;
        }
    }

    if (!isMoveInProcess_ && isUniqueWayToNewPosition_) {
        isUniqueWayToNewPosition_ = false;
        checkers_.makeMove(oneWayMove_);
        copyBoard_ = checkers_.getCopyBoard();
        isSquareSelected_ = false;
        possibleMoves_.clear();
        render();  // FIXME: If we remove it, the beat process can freeze.
                   // We should correct the engine (maybe return optional) because engine can freeze the gui
    }
}

void PlayState::highlightPossibleMoves(Position pos)
{
    for (const auto& m : possibleMoves_) {
        if (m.from.row == pos.row && m.from.col == pos.col) {
            const Move* move = &m;
            do {
                highlightCircle_.setPosition(move->to.col * tile_ + offsetForHighlight_,
                                             move->to.row * tile_ + offsetForHighlight_);
                window_.draw(highlightCircle_);
                move = move->nextMove.get();
            } while (move);
        }
    }
}

void PlayState::drawPiece(Position pos, bool isMoving)
{
    auto piece = copyBoard_(pos);
    sf::CircleShape& shape = (piece.getColour() == COLOUR::WHITE ? whitePieceCircle_ : blackPieceCircle_);
    sf::Vector2f currentPos(pos.col * tile_ + offsetForPiece_, pos.row * tile_ + offsetForPiece_);

    if (isMoving) {
        sf::Vector2f startPos(lastSelectedPosition_.col * tile_ + offsetForPiece_,
                              lastSelectedPosition_.row * tile_ + offsetForPiece_);
        sf::Vector2f endPos(makeMoveTo_.col * tile_ + offsetForPiece_, makeMoveTo_.row * tile_ + offsetForPiece_);

        movePosition_ += 0.05f;
        float t = std::min(movePosition_, 1.0f);
        currentPos = (endPos - startPos) * t + startPos;
    }
    shape.setPosition(currentPos);

    window_.draw(shape);
    if (piece.isQueen()) {
        sf::Sprite sprite(resourceManager_.getQueenTexture());
        sf::Vector2f currentPosSprite = currentPos + sf::Vector2f(offsetForQueenTexture_, offsetForQueenTexture_);

        sprite.setPosition(currentPosSprite);
        window_.draw(sprite);
    }
}

void PlayState::reset()
{
    checkers_.setCheckersType(gameContext_.checkersType);
    checkers_.reset();
    copyBoard_ = checkers_.getCopyBoard();
    lastSelectedPosition_.reset();
    makeMoveTo_.reset();
    isSquareSelected_ = false;
    isMoveInProcess_ = false;
    isUniqueWayToNewPosition_ = false;
    isGameOver_ = false;
    movePosition_ = 0.f;
    oneWayMove_.reset();
    possibleMoves_.clear();
    currentMoveColor_ = COLOUR::WHITE;
    if (gameContext_.mode == MODE::COMPUTER) {
        if (gameContext_.engineMode == ENGINE_MODE::NOVICE) {
            engine_ = std::make_unique<RandomEngine>(checkers_);
        } else {
            engine_ = std::make_unique<MinimaxEngine>(checkers_, gameContext_.engineMode);
        }
    }

    // calculate default tiles, radiuses etc
    tile_ = Game::WINDOW_WIDTH / copyBoard_.getWidth();
    int offsetForRadius;
    switch (gameContext_.checkersType) {
        case CHECKERS_TYPE::INTERNATIONAL:
            offsetForRadius = 20;
            break;

        case CHECKERS_TYPE::RUSSIAN:
            offsetForRadius = 22;
            break;

        case CHECKERS_TYPE::CANADIAN:
            offsetForRadius = 15;
            break;

        default:
            throw std::logic_error("Unknown CHECKERS_TYPE");
    }

    pieceRadius_ = (tile_ - offsetForRadius) / 2.0f;
    offsetForPiece_ = (tile_ - 2 * pieceRadius_) / 2.0f;
    highlightRadius_ = pieceRadius_ / 2;
    offsetForHighlight_ = (tile_ - 2 * highlightRadius_) / 2.0f;
    offsetForQueenTexture_ = (tile_ - 32) / 2.0f - offsetForPiece_;  // width of Texture is 32
    assert(offsetForQueenTexture_ > 0);

    highlightCircle_.setRadius(highlightRadius_);
    whitePieceCircle_.setRadius(pieceRadius_);
    blackPieceCircle_.setRadius(pieceRadius_);
    whiteSquare_.setSize(sf::Vector2f(tile_, tile_));
    blackSquare_.setSize(sf::Vector2f(tile_, tile_));
}
