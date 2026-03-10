#include "RandomEngine.hpp"

#include <ranges>
#include <stdexcept>
#include <unordered_map>
#include <vector>

#include "Checkers.hpp"

RandomEngine::RandomEngine(Checkers& checkers) : Engine(checkers)
{
}

Move RandomEngine::getBestMove()
{
    const auto& validMoves = checkers_.getValidMoves();
    if (const auto size = validMoves.size(); size > 0) {
        const auto moveIndex = mt() % size;
        int i = 0;
        for (const auto& moves : validMoves | std::views::values) {
            if (i == static_cast<int>(moveIndex)) {
                return cloneMove(moves[mt() % moves.size()]);
            }
            ++i;
        }
    }
    throw std::runtime_error("Engine cannot produce the valid move");
}
