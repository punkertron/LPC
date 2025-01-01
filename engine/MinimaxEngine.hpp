#pragma once

#include <array>
#include <random>

#include "Engine.hpp"

class MinimaxEngine final : public Engine {
private:
    const int maxDepth_;
    std::mt19937 mt{std::random_device{}()};
    std::uniform_real_distribution<float> dist{-0.3f, 0.3f};

    float EvaluatePositionRecursive(int depth, Board& curBoard, bool isMaximizingPlayer);

public:
    MinimaxEngine(Board& board, ENGINE_MODE mode);
    Move getBestMove() override;
};
