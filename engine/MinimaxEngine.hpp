#pragma once

#include <array>

#include "Engine.hpp"

class MinimaxEngine final : public Engine {
private:
    const int maxDepth_;

    float EvaluatePositionRecursive(int depth, Board& curBoard, bool isMaximizingPlayer);

public:
    MinimaxEngine(Board& board, ENGINE_MODE mode);
    Move getBestMove() override;
};
