#include "RandomEngine.hpp"

#include <stdexcept>

RandomEngine::RandomEngine(Board& board) : Engine(board)
{
}

Move RandomEngine::getBestMove()
{
    auto validMoves = board_.getValidMoves();
    int size = validMoves.size();
    int moveIndex = mt() % size;
    int i = 0;
    for (auto cit = validMoves.cbegin(), cend = validMoves.cend(); cit != cend; ++cit) {
        if (i == moveIndex) {
            return cit->second[mt() % cit->second.size()];
        }
        ++i;
    }
    throw std::runtime_error("Engine cannot get the valid move");
}
