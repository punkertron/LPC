#pragma once

#include <random>

#include "Engine.hpp"
#include "Move.hpp"

class Checkers;

class RandomEngine final : public Engine {
private:
    std::mt19937 mt{std::random_device{}()};

public:
    RandomEngine(Checkers& checkers);
    Move getBestMove() override;
};
