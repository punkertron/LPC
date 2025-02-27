/**
 * Implementation of the MinimaxEngine class using the Minimax algorithm with Alpha-Beta pruning.
 * More about Alpha-Beta pruning: https://www.youtube.com/watch?v=l-hh51ncgDI
 *
 * The MinimaxEngine uses the Minimax algorithm to evaluate possible moves and select the optimal one.
 * It recursively explores potential moves up to a specified depth, alternating between maximizing
 * and minimizing players to simulate both sides' strategies. The evaluation function assesses
 * the desirability of board states, allowing the engine to choose moves that maximize its advantage
 * while minimizing the opponent's.
 *
 *
 * isMaximizingPlayer == true - white move
 * isMaximizingPlayer == false - black move
 *
 */

#include "MinimaxEngine.hpp"

#include <algorithm>
#include <array>
#include <cmath>
#include <limits>
#include <stdexcept>
#include <unordered_map>
#include <utility>
#include <vector>

#include "Checkers.hpp"

struct Board;

constexpr std::array<int, 4> maxDepthsArr = []() consteval {
    std::array<int, 4> arr = {};
    // NO NOVICE HERE AND THIS IS CORRECT
    arr[static_cast<int>(ENGINE_MODE::EASY)] = 2;
    arr[static_cast<int>(ENGINE_MODE::MEDIUM)] = 4;
    arr[static_cast<int>(ENGINE_MODE::HARD)] = 6;
    arr[static_cast<int>(ENGINE_MODE::GRANDMASTER)] = 8;
    return arr;
}();

static inline int endgineModeToInt(ENGINE_MODE mode)
{
    return static_cast<int>(mode);
}

float evaluatePosition(const Board& board);

MinimaxEngine::MinimaxEngine(Checkers& checkers, ENGINE_MODE mode) :
    Engine(checkers),
    maxDepth_{mode == ENGINE_MODE::NOVICE
                  ? throw std::logic_error("MinimaxEngine doesn't implement NOVICE mode. Use Random Engine instead")
                  : maxDepthsArr[endgineModeToInt(mode)]}
{
}

static inline float getDefaultScore(bool isMaximizingPlayer)
{
    if (isMaximizingPlayer) {
        return std::numeric_limits<float>::lowest();
    } else {
        return std::numeric_limits<float>::max();
    }
}

// Recursive Minimax function
float MinimaxEngine::EvaluatePositionRecursive(int depth, Checkers& curBoard, bool isMaximizingPlayer, float alpha,
                                               float beta)
{
    if (auto result = curBoard.getResult(); result.isOver) {
        if (result.winner == COLOUR::WHITE) {
            return 1000.0f;
        } else {
            return -1000.0f;
        }
    }
    if (depth >= maxDepth_) {
        return evaluatePosition(curBoard.getBoard());
    }

    auto moves = curBoard.getValidMoves();
    if (moves.empty()) {
        return evaluatePosition(curBoard.getBoard());
    }

    // Initialize bestScore based on whether we're maximizing or minimizing
    float bestScore = getDefaultScore(isMaximizingPlayer);

    for (const auto& moveSeries : moves) {
        for (const auto& move : moveSeries.second) {
            // Clone the current board to simulate the move
            Checkers newBoard = curBoard;
            newBoard.makeMove(move);

            // Recursively evaluate the new board state
            float currentScore = EvaluatePositionRecursive(depth + 1, newBoard, !isMaximizingPlayer, alpha, beta);

            // Update bestScore based on maximizing or minimizing
            if (isMaximizingPlayer) {
                bestScore = std::max(bestScore, currentScore);
                alpha = std::max(alpha, bestScore);
            } else {
                bestScore = std::min(bestScore, currentScore);
                beta = std::min(beta, bestScore);
            }
            if (beta <= alpha) {
                break;
            }
        }
        if (beta <= alpha) {
            break;
        }
    }

    return bestScore;
}

Move MinimaxEngine::getBestMove()
{
    // Doesn't do anything if there is only one possible move
    auto moves = checkers_.getValidMoves();
    if (moves.size() == 1 && (*moves.begin()).second.size() == 1) {
        return moves.begin()->second[0];
    }

    // Determine if the current player is maximizing or minimizing
    bool isMaximizingPlayer = checkers_.getCurrentColour() == COLOUR::WHITE;
    float bestScore = getDefaultScore(isMaximizingPlayer);

    Move bestMove;

    for (const auto& moveSeries : moves) {
        for (const auto& move : moveSeries.second) {
            // Clone the current board to simulate the move
            Checkers newBoard = checkers_;
            newBoard.makeMove(move);

            // Evaluate the move using the recursive function
            float alpha = getDefaultScore(true);
            float beta = getDefaultScore(false);
            float currentScore = EvaluatePositionRecursive(1, newBoard, !isMaximizingPlayer, alpha, beta);

            // Simulate random decision making if the moves are approximately equal in strength
            // This is to minimize the probability of completely identical games by making the same moves
            if (std::abs(currentScore - bestScore) < 0.5f) {
                currentScore += dist(mt);
            }

            // Update the bestMove if a better score is found
            if (isMaximizingPlayer) {
                if (currentScore > bestScore) {
                    bestScore = currentScore;
                    bestMove = move;
                }
                alpha = std::max(alpha, bestScore);
            } else {
                if (currentScore < bestScore) {
                    bestScore = currentScore;
                    bestMove = move;
                }
                beta = std::min(beta, bestScore);
            }
        }
    }

    return bestMove;
}
