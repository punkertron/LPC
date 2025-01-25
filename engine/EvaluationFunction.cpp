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
#include <cassert>
#include <memory>
#include <stdexcept>
#include <vector>

#include "Board.hpp"
#include "Checkers.hpp"
#include "Piece.hpp"

//////////////
// EIGHTxEIGHT
constexpr std::array<float, 8> rowValuesForRegularPieceEIGHTxEIGHT = {
    {0, 0, 0, 0.2f, 0.4f, 0.5f, 0.7f, 1.f}
};

constexpr std::array<float, 8> rowValuesForQueenEIGHTxEIGHT = {
    {0, 0.3f, 0.4f, 0.5f, 0.5f, 0.4f, 0.3f, 0.f}
};

constexpr std::array<float, 4> columnValuesEIGHTxEIGHT = {
    {0, 0.06f, 0.06f, 0}
};
// EIGHTxEIGHT

//////////////
// TENxTEN
constexpr std::array<float, 10> rowValuesForRegularPieceTENxTEN = {
    {0, 0, 0, 0, 0.2f, 0.3f, 0.4f, 0.5f, 0.7f, 1.f}
};

constexpr std::array<float, 10> rowValuesForQueenTENxTEN = {
    {0, 0.3f, 0.4f, 0.5f, 0.6f, 0.6f, 0.5f, 0.4f, 0.3f, 0.f}
};

constexpr std::array<float, 5> columnValuesTENxTEN = {
    {0, 0.06f, 0.08f, 0.06f, 0}
};
// TENxTEN

//////////////
// TWELVExTWELVE
constexpr std::array<float, 12> rowValuesForRegularPieceTWELVExTWELVE = {
    {0, 0, 0, 0, 0.2f, 0.3f, 0.4f, 0.5f, 0.6f, 0.7f, 0.8f, 1.f}
};

constexpr std::array<float, 12> rowValuesForQueenTWELVExTWELVE = {
    {0, 0.3f, 0.4f, 0.5f, 0.6f, 0.7f, 0.7f, 0.6f, 0.5f, 0.4f, 0.3f, 0.f}
};

constexpr std::array<float, 6> columnValuesTWELVExTWELVE = {
    {0, 0.06f, 0.08f, 0.8f, 0.06f, 0}
};
// TWELVExTWELVE

static inline float getPieceValue(const Piece p)
{
    if (p.isRegular()) {
        return 3.0f;
    } else {  // QUEEN
        return 12.0f;
    }
}

static inline float getRowValue(const BOARD_TYPE bt, const Piece p, int i)
{
    if (p.isRegular()) {
        switch (bt) {
            case BOARD_TYPE::EIGHTxEIGHT:
                assert(i < 8);
                return rowValuesForRegularPieceEIGHTxEIGHT[i];

            case BOARD_TYPE::TENxTEN:
                assert(i < 10);
                return rowValuesForRegularPieceTENxTEN[i];

            case BOARD_TYPE::TWELVExTWELVE:
                assert(i < 12);
                return rowValuesForRegularPieceTWELVExTWELVE[i];

            default:
                throw std::logic_error("Unknown BOARD_TYPE in EvaluationFunction");
        }
    } else {  // Queen
        switch (bt) {
            case BOARD_TYPE::EIGHTxEIGHT:
                assert(i < 8);
                return rowValuesForQueenEIGHTxEIGHT[i];

            case BOARD_TYPE::TENxTEN:
                assert(i < 10);
                return rowValuesForQueenTENxTEN[i];

            case BOARD_TYPE::TWELVExTWELVE:
                assert(i < 12);
                return rowValuesForQueenTWELVExTWELVE[i];

            default:
                throw std::logic_error("Unknown BOARD_TYPE in EvaluationFunction");
        }
    }
}

static inline float getColumnValue(const BOARD_TYPE bt, int j)
{
    switch (bt) {
        case BOARD_TYPE::EIGHTxEIGHT:
            assert(j < 8);
            return columnValuesEIGHTxEIGHT[j / 2];

        case BOARD_TYPE::TENxTEN:
            assert(j < 10);
            return columnValuesTENxTEN[j / 2];

        case BOARD_TYPE::TWELVExTWELVE:
            assert(j < 12);
            return columnValuesTWELVExTWELVE[j / 2];

        default:
            throw std::logic_error("Unknown BOARD_TYPE in EvaluationFunction");
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
                    whiteSum += getRowValue(board.getBoardType(), piece, board.getWidth() - 1 - i);
                    whiteSum += getColumnValue(board.getBoardType(), j);
                } else {
                    blackSum += getPieceValue(piece);
                    blackSum += getRowValue(board.getBoardType(), piece, i);
                    blackSum += getColumnValue(board.getBoardType(), j);
                }
            }
        }
    }
    return whiteSum - blackSum;
}
