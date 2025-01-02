#pragma once

#include "Move.hpp"

class Board;

enum class ENGINE_MODE {
    NOVICE = -1,  // yes, this is -1 and EASE = 0
    EASY = 0,
    MEDIUM,
    HARD,
    GRANDMASTER
};

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
