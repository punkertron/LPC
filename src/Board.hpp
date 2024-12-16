#pragma once

#include <array>
#include <iostream>
#include <memory>
#include <unordered_map>
#include <vector>

#include "Move.hpp"
#include "Piece.hpp"

class Board {
private:
    std::array<std::array<std::shared_ptr<Piece>, 8>, 8> board_;
    std::unordered_map<Position, std::vector<Move> > validMoves_;
    COLOUR currentMove_{COLOUR::WHITE};

    bool isWithinBoard(const Position& p) const;
    void addOneStepMoves(const Position& p, std::vector<Move>& res) const;
    void addBeatMoves(const Position& p, std::vector<Move>& res) const;
    void generateValidMoves();
    void findCaptures(const Position& p, std::array<std::array<std::shared_ptr<Piece>, 8>, 8>& boardCopy,
                      std::vector<Move>& moves) const;

public:
    Board();

    void printBoard() const;  // debug only
    std::vector<Move> generateValidMoves(const Position& p) const;
    void makeMove(const Move& m);
};
