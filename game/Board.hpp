#pragma once

#include <iostream>
#include <memory>
#include <unordered_map>
#include <vector>

#include "Move.hpp"
#include "Piece.hpp"

class Board {
private:
    std::vector<std::vector<std::shared_ptr<Piece>>> board_;
    std::unordered_map<Position, std::vector<Move>> validMoves_;
    COLOUR currentMove_{COLOUR::WHITE};

    bool isWithinBoard(const Position& p) const;
    void addOneStepMoves(const Position& p, std::vector<Move>& res) const;
    void addBeatMoves(const Position& p, std::vector<Move>& res) const;
    void generateValidMoves();
    void findCaptures(const Position& p, std::vector<std::vector<std::shared_ptr<Piece>>>& boardCopy,
                      std::vector<Move>& moves) const;

public:
    Board();

    void printBoard() const;  // debug only
    std::vector<Move> generateValidMoves(const Position& p) const;
    void makeMove(const Move& m);
    std::vector<std::vector<std::shared_ptr<Piece>>> getBoard() const;
};
