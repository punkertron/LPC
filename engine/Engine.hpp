#pragma once

#include "Board.hpp"
#include "Move.hpp"

class Board;

class Engine {
protected:
    Board& board_;

public:
    Engine(Board& board) : board_{board}
    {
    }

    virtual ~Engine() = default;
    virtual Move getBestMove() = 0;
};
