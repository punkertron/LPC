#pragma once

#include <memory>

struct Position {
    int row;
    int col;
    bool operator==(const Position& other) const
    {
        return row == other.row && col == other.col;
    }
};

namespace std
{
template <>
struct hash<Position> {
    size_t operator()(const Position& pos) const
    {
        return hash<int>()(pos.row) ^ (hash<int>()(pos.col) << 1);
    }
};
}  // namespace std

struct Move {
    Position from;
    Position to;
    bool isComplex{false};           // if there are beat moves
    std::shared_ptr<Move> nextMove;  // next move in the chains of moves
};