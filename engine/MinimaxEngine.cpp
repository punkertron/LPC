/**
 * Implementation of the MinimaxEngine class using the Minimax algorithm for decision making.
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
#include <numeric>

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

float evaluatePosition(const std::vector<std::vector<std::shared_ptr<Piece>>>& board);

MinimaxEngine::MinimaxEngine(Board& board, ENGINE_MODE mode) :
    Engine(board),
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
float MinimaxEngine::EvaluatePositionRecursive(int depth, Board& curBoard, bool isMaximizingPlayer)
{
    if (depth >= maxDepth_ /*|| curBoard.isGameOver()*/) {
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
            Board newBoard = curBoard;
            newBoard.makeMove(move);

            // Recursively evaluate the new board state
            float currentScore = EvaluatePositionRecursive(depth + 1, newBoard, !isMaximizingPlayer);

            // Update bestScore based on maximizing or minimizing
            if (isMaximizingPlayer) {
                bestScore = std::max(bestScore, currentScore);
            } else {
                bestScore = std::min(bestScore, currentScore);
            }
        }
    }

    return bestScore;
}

Move MinimaxEngine::getBestMove()
{
    // Determine if the current player is maximizing or minimizing
    bool isMaximizingPlayer = board_.getCurrentColour() == COLOUR::WHITE;
    float bestScore = getDefaultScore(isMaximizingPlayer);
    Move bestMove;

    auto moves = board_.getValidMoves();
    if (moves.size() == 1 && (*moves.begin()).second.size() == 1) {
        return moves.begin()->second[0];
    }
    for (const auto& moveSeries : moves) {
        for (const auto& move : moveSeries.second) {
            // Clone the current board to simulate the move
            Board newBoard = board_;
            newBoard.makeMove(move);

            // Evaluate the move using the recursive function
            float currentScore = EvaluatePositionRecursive(1, newBoard, !isMaximizingPlayer);

            // Update the bestMove if a better score is found
            if (isMaximizingPlayer) {
                if (currentScore > bestScore) {
                    bestScore = currentScore;
                    bestMove = move;
                }
            } else {
                if (currentScore < bestScore) {
                    bestScore = currentScore;
                    bestMove = move;
                }
            }
        }
    }

    return bestMove;
}
