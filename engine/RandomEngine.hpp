#pragma once

#include <random>

#include "Engine.hpp"

class RandomEngine final : public Engine {
private:
    std::mt19937 mt{std::random_device{}()};

public:
    RandomEngine(Board& board);
    Move getBestMove() override;
};
