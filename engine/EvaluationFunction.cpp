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
 * - `evaluatePosition(const const Board& board)`:
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
#include <vector>

#include "Board.hpp"
#include "Checkers.hpp"
#include "Piece.hpp"

constexpr std::array<float, 8> rowValuesForRegularPiece = {
    {0, 0, 0, 0.2f, 0.4f, 0.5f, 0.7f, 1.f}
};

constexpr std::array<float, 8> rowValuesForQueen = {
    {0, 0.3f, 0.4f, 0.5f, 0.5f, 0.4f, 0.3f, 0.f}
};

constexpr std::array<float, 4> columnValues = {
    {0, 0.06f, 0.06f, 0}
};

static inline float getRowValue(const Piece p, int i)
{
    if (p.isRegular()) {
        return rowValuesForRegularPiece[i];
    } else {
        return rowValuesForQueen[i];
    }
}

static inline float getPieceValue(const Piece p)
{
    if (p.isRegular()) {
        return 3.0f;
    } else {  // QUEEN
        return 12.0f;
    }
}

float evaluatePosition(const Board& board)
{
    float whiteSum{0};
    float blackSum{0};

    for (int i = 0; i < board.getWidth(); ++i) {
        for (int j = (i % 2 ? 0 : 1); j < board.getWidth(); j += 2) {
            if (auto piece = board(i, j); piece.isNotEmpty()) {
                if (piece.getColour() == COLOUR::WHITE) {
                    whiteSum += getPieceValue(piece);
                    whiteSum += getRowValue(piece, board.getWidth() - 1 - i);
                    whiteSum += columnValues[j / 2];
                } else {
                    blackSum += getPieceValue(piece);
                    blackSum += getRowValue(piece, i);
                    blackSum += columnValues[j / 2];
                }
            }
        }
    }
    return whiteSum - blackSum;
}
