/**
 * Evaluates the current game board position by assigning values to pieces and their positions.
 *
 * This file includes:
 *
 * - **Piece Values:** Defines the base values for different types of pieces (e.g., regular, queen).
 * - **Positional Modifiers:** Assigns additional value based on the row and column positions of each piece.
 * - **Position Evaluation:** Implements the `evaluatePosition` function, which calculates a numerical score
 *   representing the advantage of one player over the other.
 *
 * ## Components
 * - `evaluatePosition(const std::vector<std::vector<std::shared_ptr<Piece>>>& board)`:
 *   Iterates through the game board, sums the values of white and black pieces including positional modifiers,
 *   and returns the difference as the overall position score.
 *
 * ## Usage
 *
 * The `evaluatePosition` function can be utilized by game AI algorithms to determine the desirability
 * of a board state. By computing a score that reflects the relative advantage of one player over the other,
 * the AI can make informed decisions about which moves to prioritize.
 */

#include <array>
#include <memory>
#include <unordered_map>
#include <vector>

#include "Board.hpp"
#include "Piece.hpp"

constexpr std::array<float, 2> pieceValues = []() consteval {
    std::array<float, 2> arr = {};
    arr[static_cast<int>(PIECE_TYPE::REGULAR)] = 3.0f;
    arr[static_cast<int>(PIECE_TYPE::QUEEN)] = 12.0f;
    return arr;
}();

static inline int pieceTypeToInt(PIECE_TYPE type)
{
    return static_cast<int>(type);
}

constexpr std::array<float, 8> rowValuesForRegularPiece = {
    {0, 0, 0, 0.2f, 0.4f, 0.5f, 0.7f, 1.f}
};

constexpr std::array<float, 8> rowValuesForQueen = {
    {0, 0.3f, 0.4f, 0.5f, 0.5f, 0.4f, 0.3f, 0.f}
};

constexpr std::array<float, 4> columnValues = {
    {0, 0.06f, 0.06f, 0}
};

static inline float getRowValue(PIECE_TYPE type, int i)
{
    if (type == PIECE_TYPE::REGULAR) {
        return rowValuesForRegularPiece[i];
    } else {
        return rowValuesForQueen[i];
    }
}

float evaluatePosition(const std::vector<std::vector<std::shared_ptr<Piece>>>& board)
{
    float whiteSum{0};
    float blackSum{0};

    for (int i = 0; i < board.size(); ++i) {
        for (int j = 0; j < board[0].size(); ++j) {
            if (auto cell = board[i][j]; cell && cell->getPieceType() != PIECE_TYPE::CAPTURED) {
                if (cell->getColour() == COLOUR::WHITE) {
                    whiteSum += pieceValues[pieceTypeToInt(cell->getPieceType())];
                    whiteSum += getRowValue(cell->getPieceType(), board.size() - 1 - i);
                    whiteSum += columnValues[j / 2];
                } else {
                    blackSum += pieceValues[pieceTypeToInt(cell->getPieceType())];
                    blackSum += getRowValue(cell->getPieceType(), i);
                    blackSum += columnValues[j / 2];
                }
            }
        }
    }
    return whiteSum - blackSum;
}
