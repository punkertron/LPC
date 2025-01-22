#pragma once

#include "Move.hpp"

class Checkers;

enum class ENGINE_MODE {
    NOVICE = -1,  // yes, this is -1 and EASE = 0
    EASY = 0,
    MEDIUM,
    HARD,
    GRANDMASTER
};

class Engine {
protected:
    Checkers& checkers_;

public:
    Engine(Checkers& checkers) : checkers_{checkers}
    {
    }

    virtual ~Engine() = default;
    virtual Move getBestMove() = 0;
};
