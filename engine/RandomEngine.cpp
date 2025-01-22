#include "RandomEngine.hpp"

#include <stdexcept>
#include <unordered_map>
#include <utility>
#include <vector>

#include "Checkers.hpp"

RandomEngine::RandomEngine(Checkers& checkers) : Engine(checkers)
{
}

Move RandomEngine::getBestMove()
{
    auto validMoves = checkers_.getValidMoves();
    int size = validMoves.size();
    int moveIndex = mt() % size;
    int i = 0;
    for (auto cit = validMoves.cbegin(), cend = validMoves.cend(); cit != cend; ++cit) {
        if (i == moveIndex) {
            return cit->second[mt() % cit->second.size()];
        }
        ++i;
    }
    throw std::runtime_error("Engine cannot produce the valid move");
}
