#pragma once

#include <unordered_map>

enum class PIECE_TYPE {
    REGULAR,
    QUEEN
};

static const std::unordered_map<PIECE_TYPE, int> pieceTypeMap{
    {PIECE_TYPE::REGULAR, 1},
    {PIECE_TYPE::QUEEN,   2}
};

enum class COLOUR {
    WHITE,
    BLACK
};

static const std::unordered_map<COLOUR, int> pieceColourMap{
    {COLOUR::WHITE, 0},
    {COLOUR::BLACK, 2}
};

class Piece {
private:
    PIECE_TYPE t_;
    const COLOUR c_;

public:
    explicit Piece(PIECE_TYPE t, COLOUR c) : t_(t), c_(c)
    {
    }

    int getPieceCode() const
    {
        return pieceTypeMap.at(t_) + pieceColourMap.at(c_);
    }

    PIECE_TYPE getPieceType() const
    {
        return t_;
    }

    COLOUR getColour() const
    {
        return c_;
    }
};
