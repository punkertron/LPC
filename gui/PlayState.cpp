#include "PlayState.hpp"

#include "Game.hpp"

constexpr int TILE = Game::WINDOW_WIDTH / Game::BOARD_SIZE;
constexpr float PIECE_RADIUS = (TILE - 20) / 2.0f;
constexpr float HIGHLIGHT_RADIUS = PIECE_RADIUS / 2;
const sf::Color LIGHT_COLOR = sf::Color(240, 217, 181);
const sf::Color DARK_COLOR = sf::Color(181, 136, 99);
const sf::Color LIGHT_PIECE = sf::Color(240, 240, 210);
const sf::Color DARK_PIECE = sf::Color(75, 75, 75);
const sf::Color highlightColor(0, 255, 0, 150);  // Semi-transparent green

PlayState::PlayState(sf::RenderWindow& window, ResourceManager& resourceManager) : State{window, resourceManager}
{
    copyBoard_ = board_.getBoard();
}

Position PlayState::getPositionOnBoardFromMouse(int x, int y)
{
    int row = y / TILE;
    int col = x / TILE;
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
    if (event.type == sf::Event::MouseButtonPressed) {
        if (event.mouseButton.button == sf::Mouse::Left) {
            int x = event.mouseButton.x;
            int y = event.mouseButton.y;

            if (!isSquareSelected_) {
                lastSelectedPosition_ = getPositionOnBoardFromMouse(x, y);
                possibleMoves_ = board_.generateValidMoves(lastSelectedPosition_);
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
                    possibleMoves_ = board_.generateValidMoves(lastSelectedPosition_);
                }
            }
        }
    }
}

void PlayState::update()
{
}

void PlayState::drawBoard()
{
    for (int row = 0; row < Game::BOARD_SIZE; ++row) {
        for (int col = 0; col < Game::BOARD_SIZE; ++col) {
            sf::RectangleShape square{
                sf::Vector2f{TILE, TILE}
            };

            square.setPosition(col * TILE, row * TILE);
            square.setFillColor((row + col) % 2 == 0 ? LIGHT_COLOR : DARK_COLOR);
            window_.draw(square);
        }
    }
}

void PlayState::render()
{
    drawBoard();

    // draw pieces
    for (int row = 0; row < Game::BOARD_SIZE; ++row) {
        for (int col = 0; col < Game::BOARD_SIZE; ++col) {
            if (auto cell = copyBoard_[row][col]) {
                if (!isMoveInProcess_ || !(lastSelectedPosition_ == Position{row, col})) {
                    drawPiece(Position{row, col});
                }
            }

            if ((!isMoveInProcess_ || !isUniqueWayToNewPosition_) && !possibleMoves_.empty()) {
                highlightPossibleMoves({row, col});
            }
        }
    }

    if (isMoveInProcess_) {
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
            std::swap(copyBoard_[makeMoveTo_.row][makeMoveTo_.col],
                      copyBoard_[lastSelectedPosition_.row][lastSelectedPosition_.col]);
            lastSelectedPosition_ = makeMoveTo_;
        }
    }

    if (!isMoveInProcess_ && isUniqueWayToNewPosition_) {
        isUniqueWayToNewPosition_ = false;
        board_.makeMove(oneWayMove_);
        copyBoard_ = board_.getBoard();
        isSquareSelected_ = false;
        possibleMoves_.clear();
    }
}

void PlayState::highlightPossibleMoves(Position pos)
{
    for (const auto& m : possibleMoves_) {
        if (m.from.row == pos.row && m.from.col == pos.col) {
            const Move* move = &m;
            do {
                sf::CircleShape highlight(HIGHLIGHT_RADIUS);
                highlight.setPosition(24 + move->to.col * TILE, 24 + move->to.row * TILE);
                highlight.setFillColor(highlightColor);
                window_.draw(highlight);
                move = move->nextMove.get();
            } while (move);
        }
    }
}

void PlayState::drawPiece(Position pos, bool isMoving)
{
    auto cell = copyBoard_[pos.row][pos.col];
    sf::CircleShape piece{PIECE_RADIUS, 50};
    sf::Vector2f currentPos(pos.col * TILE + 10, pos.row * TILE + 10);
    piece.setFillColor(cell->getColour() == COLOUR::WHITE ? LIGHT_PIECE : DARK_PIECE);

    if (isMoving) {
        sf::Vector2f startPos(lastSelectedPosition_.col * TILE + 10, lastSelectedPosition_.row * TILE + 10);
        sf::Vector2f endPos(makeMoveTo_.col * TILE + 10, makeMoveTo_.row * TILE + 10);

        movePosition_ += 0.05f;
        float t = std::min(movePosition_, 1.0f);
        currentPos = (endPos - startPos) * t + startPos;
    }
    piece.setPosition(currentPos);

    window_.draw(piece);
    if (cell->getPieceType() == PIECE_TYPE::QUEEN) {
        piece.setTexture(&resourceManager_.getQueenTexture());
        sf::Sprite sprite(resourceManager_.getQueenTexture());
        sf::Vector2f currentPosSprite = currentPos + sf::Vector2f{12, 10};

        sprite.setPosition(currentPosSprite);
        window_.draw(sprite);
    }
}
