#pragma once

#include <random>

#include "Engine.hpp"
#include "Move.hpp"

class Board;

class RandomEngine final : public Engine {
private:
    std::mt19937 mt{std::random_device{}()};

public:
    RandomEngine(Board& board);
    Move getBestMove() override;
};
