#pragma once

#include <memory>

#include "Position.hpp"

struct Move {
    Position from;
    Position to;
    Position beatenPiecePos{-1, -1};
    std::shared_ptr<Move> nextMove{nullptr};  // next move in the chains of moves

    void reset()
    {
        from.reset();
        to.reset();
        beatenPiecePos.reset();
        nextMove = nullptr;
    }
};
