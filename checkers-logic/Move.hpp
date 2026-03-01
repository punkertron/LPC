#pragma once

#include <memory>

#include "Position.hpp"

struct Move {
    Position from;
    Position to;
    Position beatenPiecePos{-1, -1};
    std::unique_ptr<Move> nextMove{nullptr};  // next move in the chains of moves

    void reset()
    {
        from.reset();
        to.reset();
        beatenPiecePos.reset();
        nextMove = nullptr;
    }
};

inline std::unique_ptr<Move> cloneMovePtr(const Move& move)
{
    auto cloned = std::make_unique<Move>();
    cloned->from = move.from;
    cloned->to = move.to;
    cloned->beatenPiecePos = move.beatenPiecePos;
    if (move.nextMove) {
        cloned->nextMove = cloneMovePtr(*move.nextMove);
    }
    return cloned;
}

inline Move cloneMove(const Move& move)
{
    Move cloned;
    cloned.from = move.from;
    cloned.to = move.to;
    cloned.beatenPiecePos = move.beatenPiecePos;
    if (move.nextMove) {
        cloned.nextMove = cloneMovePtr(*move.nextMove);
    }
    return cloned;
}
