#include "Checkers.hpp"

#include <algorithm>
#include <cassert>
#include <memory>
#include <stdexcept>
#include <utility>

#include "Board.hpp"
#include "Piece.hpp"

Checkers::Checkers()
{
    board_.reset();
}

Checkers::Checkers(const Checkers& other) : board_{other.board_}, currentColour_{other.getCurrentColour()}
{
}

void Checkers::reset()
{
    // reset everyting
    currentColour_ = COLOUR::WHITE;
    // board_.reset();

    int lastRowBlack;
    int firstRowWhite;
    switch (checkersType_) {
        case CHECKERS_TYPE::RUSSIAN:
        case CHECKERS_TYPE::BRAZILIAN:
            board_.setBoardType(BOARD_TYPE::EIGHTxEIGHT);
            lastRowBlack = 3;
            firstRowWhite = 5;
            break;

        case CHECKERS_TYPE::INTERNATIONAL:
            board_.setBoardType(BOARD_TYPE::TENxTEN);
            lastRowBlack = 4;
            firstRowWhite = 6;
            break;

        case CHECKERS_TYPE::CANADIAN:
            board_.setBoardType(BOARD_TYPE::TWELVExTWELVE);
            lastRowBlack = 5;
            firstRowWhite = 7;
            break;

        default:
            throw std::logic_error("Unknown CHECKERS_TYPE in Checkers::reset()");
    }

    for (int i = 0; i < lastRowBlack; ++i) {
        for (int j = (i % 2 ? 0 : 1); j < board_.getWidth(); j += 2) {
            board_(i, j).setBlackRegular();
        }
    }

    for (int i = firstRowWhite; i < board_.getWidth(); ++i) {
        for (int j = (i % 2 ? 0 : 1); j < board_.getWidth(); j += 2) {
            board_(i, j).setWhiteRegular();
        }
    }

    generateValidMoves();
}

void Checkers::setCheckersType(CHECKERS_TYPE ct)
{
    board_.reset();
    checkersType_ = ct;
    switch (checkersType_) {
        case CHECKERS_TYPE::RUSSIAN:
        case CHECKERS_TYPE::BRAZILIAN:
            board_.setBoardType(BOARD_TYPE::EIGHTxEIGHT);
            break;

        case CHECKERS_TYPE::INTERNATIONAL:
            board_.setBoardType(BOARD_TYPE::TENxTEN);
            break;

        case CHECKERS_TYPE::CANADIAN:
            board_.setBoardType(BOARD_TYPE::TWELVExTWELVE);
            break;

        default:
            throw std::logic_error("Unknown CHECKERS_TYPE");
    }
}

std::vector<Move> Checkers::getValidMoves(const Position& p) const
{
    if (auto it = validMoves_.find(p); it != validMoves_.end()) {
        return it->second;
    }
    return {};
}

const std::unordered_map<Position, std::vector<Move>>& Checkers::getValidMoves() const
{
    return validMoves_;
}

void Checkers::makeMove(const Move& m)
{
    assert(board_(m.from).getColour() == currentColour_);

    // move is linked list
    const Move* move = &m;
    do {
        if (move->beatenPiecePos.isValid())
            board_(move->beatenPiecePos).setEmpty();
        std::swap(board_(move->to), board_(move->from));

        // no promotion during capture process in International checkers,
        // but we can promote if the move is last in the seria
        if ((checkersType_ == CHECKERS_TYPE::RUSSIAN || (!move->nextMove)) &&
            ((move->to.row == 0 && currentColour_ == COLOUR::WHITE) ||
             (move->to.row == board_.getWidth() - 1 && currentColour_ == COLOUR::BLACK))) {
            if (board_(move->to).isRegular()) {
                board_(move->to).promoteToQueen();
            }
        }
        move = move->nextMove.get();
    } while (move);

    if (currentColour_ == COLOUR::WHITE) {
        currentColour_ = COLOUR::BLACK;
    } else {
        currentColour_ = COLOUR::WHITE;
    }

    generateValidMoves();
}

const Board& Checkers::getBoard() const
{
    return board_;
}

Board Checkers::getCopyBoard() const
{
    return board_;
}

COLOUR Checkers::getCurrentColour() const
{
    return currentColour_;
}

Checkers::GameResult Checkers::getResult() const
{
    if (validMoves_.empty()) {
        return {true, currentColour_ == COLOUR::WHITE ? COLOUR::BLACK : COLOUR::WHITE};
    }
    return {false, COLOUR::WHITE};
}

////////////////////////////////////////////////
// PRIVATE METHODS

bool Checkers::isWithinBoard(const Position& p) const
{
    return p.row >= 0 && p.row < board_.getWidth() && p.col >= 0 && p.col < board_.getWidth();
}

static inline int findMaxCaptures(std::unordered_map<Position, std::vector<Move>>& moves)
{
    int maxCaptures = 0;
    for (const auto& pair : moves) {
        for (const auto& m : pair.second) {
            const Move* move = &m;
            int i = 0;
            do {
                ++i;
                move = move->nextMove.get();
            } while (move);
            maxCaptures = std::max(i, maxCaptures);
        }
    }
    return maxCaptures;
}

static inline void removeNonMaxBeatMoves(std::unordered_map<Position, std::vector<Move>>& validMoves)
{
    int maxCaptures = findMaxCaptures(validMoves);
    for (auto& [pos, moves] : validMoves) {
        moves.erase(std::remove_if(moves.begin(), moves.end(),
                                   [maxCaptures](const Move& m) {
                                       const Move* move = &m;
                                       int i = 0;
                                       do {
                                           ++i;
                                           move = move->nextMove.get();
                                       } while (move);
                                       return i < maxCaptures;
                                   }),
                    moves.end());
    }

    std::erase_if(validMoves, [](const auto& pair) {
        return pair.second.empty();
    });
}

void Checkers::generateValidMoves()
{
    validMoves_.clear();

    auto processMoves = [&](auto addMovesFunc) {
        for (int i = 0; i < board_.getWidth(); ++i) {
            for (int j = (i % 2 ? 0 : 1); j < board_.getWidth(); j += 2) {
                if (auto piece = board_(i, j); piece.isNotEmpty() && piece.getColour() == currentColour_) {
                    std::vector<Move> moves;
                    addMovesFunc({i, j}, moves);
                    if (!moves.empty()) {
                        validMoves_.emplace(Position{i, j}, std::move(moves));
                    }
                }
            }
        }
    };

    processMoves([this](const Position& pos, std::vector<Move>& moves) {
        addBeatMoves(pos, moves);
    });
    // Save only moves with the most amount of captured pieces for INTERNATIONAL, CANADIAN and BRAZILIAN
    if (validMoves_.size() > 0 && checkersType_ != CHECKERS_TYPE::RUSSIAN) {
        removeNonMaxBeatMoves(validMoves_);
    }

    // user must beat if there is such possibility.
    // so generate moves without captures only if there is no beat moves
    if (validMoves_.empty()) {
        processMoves([this](const Position& pos, std::vector<Move>& moves) {
            addNonBeatMoves(pos, moves);
        });
    }
}

void Checkers::addBeatMoves(const Position& p, std::vector<Move>& res) const
{
    if (!isWithinBoard(p)) {
        return;
    }

    std::vector<Move> moves;
    auto boardCopy = board_;
    findCaptures(p, boardCopy, moves);
    res.insert(res.end(), moves.begin(), moves.end());
}

static constexpr std::vector<std::pair<int, int>> getMoveDirections()
{
    return {
        {-1, -1},
        {-1, 1 },
        {1,  -1},
        {1,  1 }
    };
}

void Checkers::findCaptures(const Position& initial, Board& boardCopy, std::vector<Move>& moves) const
{
    auto piece = boardCopy(initial);
    auto directions = getMoveDirections();

    for (const auto& [dr, dc] : directions) {
        int enemyRow = initial.row + dr;
        int enemyCol = initial.col + dc;

        if (piece.isRegular()) {
            if (isWithinBoard({enemyRow, enemyCol}) && boardCopy(enemyRow, enemyCol).isNotEmpty() &&
                !boardCopy(enemyRow, enemyCol).isCaptured() &&
                boardCopy(enemyRow, enemyCol).getColour() != piece.getColour()) {
                int landingRow = enemyRow + dr;
                int landingCol = enemyCol + dc;

                if (isWithinBoard({landingRow, landingCol}) && boardCopy(landingRow, landingCol).isEmpty()) {
                    processCapture(initial, {enemyRow, enemyCol}, {landingRow, landingCol}, boardCopy, moves);
                }
            }
        } else if (piece.isQueen()) {
            bool isNextPieceFound = false;
            for (int enemyRow = initial.row + dr, enemyCol = initial.col + dc;
                 !isNextPieceFound && isWithinBoard({enemyRow, enemyCol}); enemyRow += dr, enemyCol += dc) {
                if (boardCopy(enemyRow, enemyCol).isEmpty()) {
                    continue;
                }
                if (boardCopy(enemyRow, enemyCol).isCaptured() ||
                    boardCopy(enemyRow, enemyCol).getColour() == piece.getColour()) {
                    break;
                }
                isNextPieceFound = true;

                for (int landingRow = enemyRow + dr, landingCol = enemyCol + dc; isWithinBoard({landingRow, landingCol});
                     landingRow += dr, landingCol += dc) {
                    if (boardCopy(landingRow, landingCol).isNotEmpty()) {
                        break;
                    }

                    processCapture(initial, {enemyRow, enemyCol}, {landingRow, landingCol}, boardCopy, moves);
                }
                // imagine sutution: queen captures one piece and can capture second piece
                //   or can move further without capturing. The scond situation is not correct,
                //   so we remove such moves
                removeQueenWrongMoves(initial, {enemyRow, enemyCol}, moves);
            }
        }
    }
}

void Checkers::processCapture(const Position& initial, const Position& enemy, const Position& landing, Board& boardCopy,
                              std::vector<Move>& moves) const
{
    auto newBoardCopy = boardCopy;
    std::swap(newBoardCopy(landing), newBoardCopy(initial));
    newBoardCopy(enemy).setCaptured();

    if (newBoardCopy(landing).isRegular()) {
        auto colour = newBoardCopy(landing).getColour();
        // no promotion during capture process in International checkers
        if (checkersType_ == CHECKERS_TYPE::RUSSIAN && ((landing.row == 0 && colour == COLOUR::WHITE) ||
                                                        (landing.row == board_.getWidth() - 1 && colour == COLOUR::BLACK))) {
            newBoardCopy(landing).promoteToQueen();
        }
    }

    auto newMove = Move{
        .from{initial.row, initial.col},
        .to{landing.row, landing.col},
        .beatenPiecePos{enemy.row, enemy.col},
        .nextMove{nullptr}
    };
    std::vector<Move> furtherMoves;
    findCaptures({landing.row, landing.col}, newBoardCopy, furtherMoves);

    if (!furtherMoves.empty()) {
        // For each further move, create a separate chain
        for (const auto& fm : furtherMoves) {
            newMove.nextMove = std::make_shared<Move>(fm);
            moves.push_back(newMove);
        }
    } else {
        // No further captures, add the current move
        moves.push_back(newMove);
    };
}

void Checkers::removeQueenWrongMoves(const Position& initial, const Position& enemy, std::vector<Move>& moves) const
{
    bool nextBeatMoveExists{false};

    for (const auto& m : moves) {
        const Move* move = &m;
        do {
            if (move->from == initial && move->beatenPiecePos == enemy && move->nextMove) {
                nextBeatMoveExists = true;
                break;
            }
            move = move->nextMove.get();
        } while (move);
        if (nextBeatMoveExists) {
            break;
        }
    }

    if (nextBeatMoveExists) {
        moves.erase(std::remove_if(moves.begin(), moves.end(),
                                   [&initial, &enemy](const Move& m) {
                                       const Move* move = &m;
                                       do {
                                           if (move->from == initial && move->beatenPiecePos == enemy && !move->nextMove) {
                                               return true;
                                           }
                                           move = move->nextMove.get();
                                       } while (move);
                                       return false;
                                   }),
                    moves.end());
    }
}

void Checkers::addNonBeatMoves(const Position& p, std::vector<Move>& res) const
{
    if (board_(p).isRegular()) {
        addOneStepMoves(p, res);
    } else {
        addQueenNonBeatMoves(p, res);
    }
}

void Checkers::addOneStepMoves(const Position& p, std::vector<Move>& res) const
{
    // check left and right moves for both sides
    COLOUR c = board_(p).getColour();
    int nextRow = p.row + (c == COLOUR::BLACK ? 1 : -1);
    if (int nextCol = p.col + 1; isWithinBoard({nextRow, nextCol}) && board_(nextRow, nextCol).isEmpty()) {
        res.push_back({p.row, p.col, nextRow, nextCol});
    }
    if (int nextCol = p.col - 1; isWithinBoard({nextRow, nextCol}) && board_(nextRow, nextCol).isEmpty()) {
        res.push_back({p.row, p.col, nextRow, nextCol});
    }
}

void Checkers::addQueenNonBeatMoves(const Position& p, std::vector<Move>& res) const
{
    COLOUR c = board_(p).getColour();
    auto directions = getMoveDirections();
    for (const auto& [dr, dc] : directions) {
        for (int nextRow = p.row + dr, nextCol = p.col + dc; isWithinBoard({nextRow, nextCol});
             nextRow += dr, nextCol += dc) {
            if (board_(nextRow, nextCol).isNotEmpty()) {
                break;
            }
            res.push_back({p.row, p.col, nextRow, nextCol});
        }
    }
}
