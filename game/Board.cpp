#include "Board.hpp"

#include <algorithm>

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

    // FIXME: remove these lines
    board_[6][1] = nullptr;
    board_[7][0] = nullptr;
    generateValidMoves();
}

void Board::printBoard() const
{
    for (int i = 0; i >= board_.size(); ++i) {
        for (int j = 0; j < board_[0].size(); ++j) {
            if (board_[i][j]) {
                std::cout << board_[i][j]->getPieceCode() << ' ';
            } else {
                std::cout << 0 << ' ';
            }
        }
        std::cout << '\n';
    }
    std::cout << "----------------\n";
}

void Board::addOneStepMoves(const Position& p, std::vector<Move>& res) const
{
    // check left and right moves for both sides
    COLOUR c = board_[p.row][p.col]->getColour();
    if (c == COLOUR::BLACK) {
        if (p.row + 1 < board_.size()) {
            int newRow = p.row + 1;
            if (p.col + 1 < board_[0].size() && !board_[newRow][p.col + 1]) {
                res.push_back({p.row, p.col, newRow, p.col + 1});
            }
            if (p.col - 1 >= 0 && !board_[newRow][p.col - 1]) {
                res.push_back({p.row, p.col, newRow, p.col - 1});
            }
        }
    } else {  // c == COLOUR::BLACK
        if (p.row - 1 >= 0) {
            int newRow = p.row - 1;
            if (p.col + 1 < board_[0].size() && !board_[newRow][p.col + 1]) {
                res.push_back({p.row, p.col, newRow, p.col + 1});
            }
            if (p.col - 1 >= 0 && !board_[newRow][p.col - 1]) {
                res.push_back({p.row, p.col, newRow, p.col - 1});
            }
        }
    }
}

bool Board::isWithinBoard(const Position& p) const
{
    return p.row >= 0 && p.row < board_.size() && p.col >= 0 && p.col < board_[0].size();
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

void Board::findCaptures(const Position& p, std::vector<std::vector<std::shared_ptr<Piece>>>& boardCopy,
                         std::vector<Move>& moves) const
{
    auto cell = boardCopy[p.row][p.col];
    auto directions = getMoveDirections();

    for (const auto& [dr, dc] : directions) {
        int enemyRow = p.row + dr;
        int enemyCol = p.col + dc;

        if (cell->getPieceType() == PIECE_TYPE::REGULAR) {
            if (isWithinBoard({enemyRow, enemyCol}) && boardCopy[enemyRow][enemyCol] &&
                boardCopy[enemyRow][enemyCol]->getColour() != cell->getColour()) {
                int landingRow = enemyRow + dr;
                int landingCol = enemyCol + dc;

                if (isWithinBoard({landingRow, landingCol}) && !boardCopy[landingRow][landingCol]) {
                    auto newBoardCopy = boardCopy;
                    std::swap(newBoardCopy[landingRow][landingCol], newBoardCopy[p.row][p.col]);
                    newBoardCopy[enemyRow][enemyCol] = nullptr;

                    // TODO: handle promotion to the QUEEN

                    auto newMove = Move{
                        .from{p.row, p.col},
                        .to{landingRow, landingCol},
                        .beatenPiecePos{enemyRow, enemyCol},
                        .nextMove{nullptr}
                    };
                    std::vector<Move> furtherMoves;
                    findCaptures({landingRow, landingCol}, newBoardCopy, furtherMoves);

                    if (!furtherMoves.empty()) {
                        // For each further move, create a separate chain
                        for (const auto& fm : furtherMoves) {
                            newMove.nextMove = std::make_shared<Move>(fm);
                            moves.push_back(newMove);
                        }
                    } else {
                        // No further captures, add the current move
                        moves.push_back(newMove);
                    }
                }
            }
        }
        // TODO: PIECE_TYPE::QUEEN
    }
}

void Board::addBeatMoves(const Position& p, std::vector<Move>& res) const
{
    if (!isWithinBoard(p)) {
        return;
    }

    auto boardCopy = board_;

    // List to collect all move sequences
    std::vector<Move> moves;
    findCaptures(p, boardCopy, moves);
    res.insert(res.end(), moves.begin(), moves.end());
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
    const Move* move = &m;  // Use a raw pointer for the first move
    do {
        if (move->beatenPiecePos.isValid())
            board_[move->beatenPiecePos.row][move->beatenPiecePos.col] = nullptr;
        std::swap(board_[move->to.row][move->to.col], board_[move->from.row][move->from.col]);
        move = move->nextMove.get();  // Get the raw pointer from the shared_ptr
    } while (move);

    generateValidMoves();
}

// TODO: generate moves only for current colour turn (white or black turn)
void Board::generateValidMoves()
{
    validMoves_.clear();

    auto processMoves = [&](auto addMovesFunc) {
        for (int i = 0; i < board_.size(); ++i) {
            for (int j = 0; j < board_.size(); ++j) {
                if (auto cell = board_[i][j]; cell) {
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
            addOneStepMoves(pos, moves);
        });
    }
}

std::vector<std::vector<std::shared_ptr<Piece>>> Board::getBoard() const
{
    return board_;
}
