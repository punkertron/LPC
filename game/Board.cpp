#include "Board.hpp"

#include <algorithm>
#include <stdexcept>

Board::Board() : board_(8, std::vector<std::shared_ptr<Piece>>(8, nullptr))
{
    for (int i = 0; i < 3; ++i) {
        for (int j = (i % 2 ? 0 : 1); j < board_[0].size(); j += 2) {
            board_[i][j] = std::make_shared<Piece>(PIECE_TYPE::REGULAR, COLOUR::BLACK);
        }
    }

    for (int i = 5; i < board_.size(); ++i) {
        for (int j = (i % 2 ? 0 : 1); j < board_[0].size(); j += 2) {
            board_[i][j] = std::make_shared<Piece>(PIECE_TYPE::REGULAR, COLOUR::WHITE);
        }
    }

    generateValidMoves();
}

std::vector<Move> Board::generateValidMoves(const Position& p) const
{
    if (auto it = validMoves_.find(p); it != validMoves_.end()) {
        return it->second;
    }
    return {};
}

void Board::makeMove(const Move& m)
{
    if (board_[m.from.row][m.from.col]->getColour() != currentColour_) {
        throw std::logic_error("Wrong colour! White must do one move, and then black must do one move");
    }

    // move is linked list
    const Move* move = &m;
    do {
        if (move->beatenPiecePos.isValid())
            board_[move->beatenPiecePos.row][move->beatenPiecePos.col] = nullptr;
        std::swap(board_[move->to.row][move->to.col], board_[move->from.row][move->from.col]);
        if ((move->to.row == 0 && currentColour_ == COLOUR::WHITE) ||
            (move->to.row == board_.size() - 1 && currentColour_ == COLOUR::BLACK)) {
            board_[move->to.row][move->to.col]->setPieceType(PIECE_TYPE::QUEEN);
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

static std::vector<std::vector<std::shared_ptr<Piece>>> createBoardCopy(
    const std::vector<std::vector<std::shared_ptr<Piece>>>& board)
{
    auto deepCopiedVec = std::vector<std::vector<std::shared_ptr<Piece>>>();

    for (const auto& innerVec : board) {
        std::vector<std::shared_ptr<Piece>> newInnerVec;
        for (const auto& piecePtr : innerVec) {
            if (piecePtr) {
                newInnerVec.push_back(std::make_shared<Piece>(*piecePtr));
            } else {
                newInnerVec.push_back(nullptr);
            }
        }
        deepCopiedVec.push_back(newInnerVec);
    }
    return deepCopiedVec;
}

std::vector<std::vector<std::shared_ptr<Piece>>> Board::getBoard() const
{
    return createBoardCopy(board_);
}

Board::GameResult Board::getResult() const
{
    if (validMoves_.empty()) {
        return {true, currentColour_ == COLOUR::WHITE ? COLOUR::BLACK : COLOUR::WHITE};
    }
    return {false, COLOUR::WHITE};
}

////////////////////////////////////////////////
// PRIVATE METHODS

bool Board::isWithinBoard(const Position& p) const
{
    return p.row >= 0 && p.row < board_.size() && p.col >= 0 && p.col < board_[0].size();
}

void Board::generateValidMoves()
{
    validMoves_.clear();

    auto processMoves = [&](auto addMovesFunc) {
        for (int i = 0; i < board_.size(); ++i) {
            for (int j = 0; j < board_.size(); ++j) {
                if (auto cell = board_[i][j]; cell && cell->getColour() == currentColour_) {
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

    // user must beat if there is such possibility.
    // so generate moves without captures only if there is no beat moves
    if (validMoves_.empty()) {
        processMoves([this](const Position& pos, std::vector<Move>& moves) {
            addNonBeatMoves(pos, moves);
        });
    }
}

void Board::addBeatMoves(const Position& p, std::vector<Move>& res) const
{
    if (!isWithinBoard(p)) {
        return;
    }

    std::vector<Move> moves;
    auto boardCopy = createBoardCopy(board_);
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

void Board::findCaptures(const Position& initial, std::vector<std::vector<std::shared_ptr<Piece>>>& boardCopy,
                         std::vector<Move>& moves) const
{
    auto cell = boardCopy[initial.row][initial.col];
    auto directions = getMoveDirections();

    for (const auto& [dr, dc] : directions) {
        int enemyRow = initial.row + dr;
        int enemyCol = initial.col + dc;

        if (cell->getPieceType() == PIECE_TYPE::REGULAR) {
            if (isWithinBoard({enemyRow, enemyCol}) && boardCopy[enemyRow][enemyCol] &&
                boardCopy[enemyRow][enemyCol]->getColour() != cell->getColour()) {
                int landingRow = enemyRow + dr;
                int landingCol = enemyCol + dc;

                if (isWithinBoard({landingRow, landingCol}) && !boardCopy[landingRow][landingCol]) {
                    processCapture(initial, {enemyRow, enemyCol}, {landingRow, landingCol}, boardCopy, moves);
                }
            }
        } else {  // PIECE_TYPE::QUEEN
            for (int enemyRow = initial.row + dr, enemyCol = initial.col + dc; isWithinBoard({enemyRow, enemyCol});
                 enemyRow += dr, enemyCol += dc) {
                if (!boardCopy[enemyRow][enemyCol]) {
                    continue;
                }
                if (boardCopy[enemyRow][enemyCol]->getColour() == cell->getColour()) {
                    break;
                }

                for (int landingRow = enemyRow + dr, landingCol = enemyCol + dc; isWithinBoard({landingRow, landingCol});
                     landingRow += dr, landingCol += dc) {
                    if (boardCopy[landingRow][landingCol]) {
                        break;
                    }

                    processCapture(initial, {enemyRow, enemyCol}, {landingRow, landingCol}, boardCopy, moves);
                }
            }
        }
    }
}

void Board::processCapture(const Position& initial, const Position& enemy, const Position& landing,
                           std::vector<std::vector<std::shared_ptr<Piece>>>& boardCopy, std::vector<Move>& moves) const
{
    auto newBoardCopy = createBoardCopy(boardCopy);
    std::swap(newBoardCopy[landing.row][landing.col], newBoardCopy[initial.row][initial.col]);
    newBoardCopy[enemy.row][enemy.col] = nullptr;

    if (newBoardCopy[landing.row][landing.col]->getPieceType() == PIECE_TYPE::REGULAR) {
        auto colour = newBoardCopy[landing.row][landing.col]->getColour();
        if ((landing.row == 0 && colour == COLOUR::WHITE) || (landing.row == board_.size() - 1 && colour == COLOUR::BLACK)) {
            newBoardCopy[landing.row][landing.col]->setPieceType(PIECE_TYPE::QUEEN);
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

void Board::addNonBeatMoves(const Position& p, std::vector<Move>& res) const
{
    if (board_[p.row][p.col]->getPieceType() == PIECE_TYPE::REGULAR) {
        addOneStepMoves(p, res);
    } else {
        addQueenNonBeatMoves(p, res);
    }
}

void Board::addOneStepMoves(const Position& p, std::vector<Move>& res) const
{
    // check left and right moves for both sides
    COLOUR c = board_[p.row][p.col]->getColour();
    int nextRow = p.row + (c == COLOUR::BLACK ? 1 : -1);
    if (int nextCol = p.col + 1; isWithinBoard({nextRow, nextCol}) && !board_[nextRow][nextCol]) {
        res.push_back({p.row, p.col, nextRow, nextCol});
    }
    if (int nextCol = p.col - 1; isWithinBoard({nextRow, nextCol}) && !board_[nextRow][nextCol]) {
        res.push_back({p.row, p.col, nextRow, nextCol});
    }
}

void Board::addQueenNonBeatMoves(const Position& p, std::vector<Move>& res) const
{
    COLOUR c = board_[p.row][p.col]->getColour();
    auto directions = getMoveDirections();
    for (const auto& [dr, dc] : directions) {
        for (int nextRow = p.row + dr, nextCol = p.col + dc; isWithinBoard({nextRow, nextCol});
             nextRow += dr, nextCol += dc) {
            if (board_[nextRow][nextCol]) {
                break;
            }
            res.push_back({p.row, p.col, nextRow, nextCol});
        }
    }
}
