#pragma once

#include <memory>
#include <unordered_map>
#include <vector>

#include "Move.hpp"
#include "Piece.hpp"

class Board {
private:
    std::vector<std::vector<std::shared_ptr<Piece>>> board_;
    std::unordered_map<Position, std::vector<Move>> validMoves_;
    COLOUR currentColour_{COLOUR::WHITE};

    bool isWithinBoard(const Position& p) const;
    void generateValidMoves();
    void addBeatMoves(const Position& p, std::vector<Move>& res) const;
    void findCaptures(const Position& initial, std::vector<std::vector<std::shared_ptr<Piece>>>& boardCopy,
                      std::vector<Move>& moves) const;
    void processCapture(const Position& initial, const Position& enemy, const Position& landing,
                        std::vector<std::vector<std::shared_ptr<Piece>>>& boardCopy, std::vector<Move>& moves) const;
    void removeQueenWrongMoves(const Position& initial, const Position& enemy, std::vector<Move>& moves) const;
    void addNonBeatMoves(const Position& p, std::vector<Move>& res) const;
    void addOneStepMoves(const Position& p, std::vector<Move>& res) const;
    void addQueenNonBeatMoves(const Position& p, std::vector<Move>& res) const;

public:
    Board();

    std::vector<Move> getValidMoves(const Position& p) const;
    std::unordered_map<Position, std::vector<Move>> getValidMoves() const;
    void makeMove(const Move& m);
    std::vector<std::vector<std::shared_ptr<Piece>>> getBoard() const;
    COLOUR getCurrentColour() const;

    struct GameResult {
        bool isOver;
        COLOUR winner;
    };
    GameResult getResult() const;
};
