#include "Checkers.hpp"

#include <algorithm>
#include <array>
#include <cassert>
#include <memory>
#include <stdexcept>
#include <utility>

#include "Board.hpp"
#include "Piece.hpp"

namespace
{
size_t darkSquaresCountFor(const Board& board)
{
    const int width = board.getWidth();
    return static_cast<size_t>((width * width) / 2);
}

constexpr std::array<std::pair<int, int>, 4> kMoveDirections{
    {{-1, -1}, {-1, 1}, {1, -1}, {1, 1}}
};

constexpr size_t kMaxHistorySize = 256;
}  // namespace

Checkers::Checkers()
{
    board_.reset();
    validMoves_.reserve(darkSquaresCountFor(board_));
}

Checkers::Checkers(const Checkers& other) :
    board_{other.board_}, currentColour_{other.getCurrentColour()}, checkersType_{other.checkersType_}
{
}

void Checkers::reset()
{
    // reset everything
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

    validMoves_.reserve(darkSquaresCountFor(board_));

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
    undoHistory_.clear();
    redoHistory_.clear();
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

    validMoves_.reserve(darkSquaresCountFor(board_));
    undoHistory_.clear();
    redoHistory_.clear();
}

std::vector<Move> Checkers::getValidMoves(const Position& p) const
{
    if (auto it = validMoves_.find(p); it != validMoves_.end()) {
        std::vector<Move> clonedMoves;
        clonedMoves.reserve(it->second.size());
        for (const auto& move : it->second) {
            clonedMoves.push_back(cloneMove(move));
        }
        return clonedMoves;
    }
    return {};
}

const std::unordered_map<Position, std::vector<Move>>& Checkers::getValidMoves() const
{
    return validMoves_;
}

void Checkers::makeMove(const Move& m)
{
    makeMoveInternal(m, true);
}

void Checkers::makeMoveWithoutHistory(const Move& m)
{
    makeMoveInternal(m, false);
}

bool Checkers::undoMove()
{
    if (undoHistory_.empty()) {
        return false;
    }

    GameStateSnapshot previousState = undoHistory_.back();
    undoHistory_.pop_back();
    if (redoHistory_.size() >= kMaxHistorySize) {
        redoHistory_.pop_front();
    }
    redoHistory_.push_back(captureSnapshot());
    restoreSnapshot(previousState);
    return true;
}

bool Checkers::redoMove()
{
    if (redoHistory_.empty()) {
        return false;
    }

    GameStateSnapshot nextState = redoHistory_.back();
    redoHistory_.pop_back();
    if (undoHistory_.size() >= kMaxHistorySize) {
        undoHistory_.pop_front();
    }
    undoHistory_.push_back(captureSnapshot());
    restoreSnapshot(nextState);
    return true;
}

bool Checkers::canUndo() const
{
    return !undoHistory_.empty();
}

bool Checkers::canRedo() const
{
    return !redoHistory_.empty();
}

void Checkers::makeMoveInternal(const Move& m, bool trackHistory)
{
    assert(board_(m.from).getColour() == currentColour_);

    if (trackHistory) {
        if (undoHistory_.size() >= kMaxHistorySize) {
            undoHistory_.pop_front();
        }
        undoHistory_.push_back(captureSnapshot());
        redoHistory_.clear();
    }

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

Checkers::GameStateSnapshot Checkers::captureSnapshot() const
{
    return GameStateSnapshot{.board = board_, .currentColour = currentColour_};
}

void Checkers::restoreSnapshot(const GameStateSnapshot& snapshot)
{
    board_ = snapshot.board;
    currentColour_ = snapshot.currentColour;
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
    const int width = board_.getWidth();
    const int darkSquaresCount = (width * width) / 2;

    std::vector<Position> playerPieces;
    playerPieces.reserve(static_cast<size_t>(darkSquaresCount));

    for (int i = 0; i < width; ++i) {
        for (int j = (i % 2 ? 0 : 1); j < width; j += 2) {
            Piece& piece = board_(i, j);
            if (piece.isEmpty() || piece.getColour() != currentColour_) {
                continue;
            }

            Position pos{i, j};
            playerPieces.push_back(pos);

            std::vector<Move> moves;
            addBeatMoves(pos, moves);
            if (!moves.empty()) {
                validMoves_.emplace(pos, std::move(moves));
            }
        }
    }

    // Save only moves with the most amount of captured pieces for INTERNATIONAL, CANADIAN and BRAZILIAN
    if (validMoves_.size() > 0 && checkersType_ != CHECKERS_TYPE::RUSSIAN) {
        removeNonMaxBeatMoves(validMoves_);
    }

    // user must beat if there is such possibility.
    // so generate moves without captures only if there is no beat moves
    if (validMoves_.empty()) {
        for (const Position& pos : playerPieces) {
            std::vector<Move> moves;
            addNonBeatMoves(pos, moves);
            if (!moves.empty()) {
                validMoves_.emplace(pos, std::move(moves));
            }
        }
    }
}

void Checkers::addBeatMoves(const Position& p, std::vector<Move>& res) const
{
    if (!isWithinBoard(p)) {
        return;
    }

    auto boardCopy = board_;
    findCaptures(p, boardCopy, res);
}

void Checkers::findCaptures(const Position& initial, Board& boardCopy, std::vector<Move>& moves) const
{
    auto piece = boardCopy(initial);

    for (const auto& [dr, dc] : kMoveDirections) {
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
            newMove.nextMove = cloneMovePtr(fm);
            moves.push_back(cloneMove(newMove));
        }
    } else {
        // No further captures, add the current move
        moves.push_back(std::move(newMove));
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
        res.push_back({
            .from{p.row,   p.col  },
            .to{nextRow, nextCol}
        });
    }
    if (int nextCol = p.col - 1; isWithinBoard({nextRow, nextCol}) && board_(nextRow, nextCol).isEmpty()) {
        res.push_back({
            .from{p.row,   p.col  },
            .to{nextRow, nextCol}
        });
    }
}

void Checkers::addQueenNonBeatMoves(const Position& p, std::vector<Move>& res) const
{
    for (const auto& [dr, dc] : kMoveDirections) {
        for (int nextRow = p.row + dr, nextCol = p.col + dc; isWithinBoard({nextRow, nextCol});
             nextRow += dr, nextCol += dc) {
            if (board_(nextRow, nextCol).isNotEmpty()) {
                break;
            }
            res.push_back({
                .from{p.row,   p.col  },
                .to{nextRow, nextCol}
            });
        }
    }
}
