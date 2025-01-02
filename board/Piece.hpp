#pragma once

enum class PIECE_TYPE {
    REGULAR,
    QUEEN,
    CAPTURED
};

enum class COLOUR {
    WHITE,
    BLACK
};

class Piece {
private:
    PIECE_TYPE t_;
    const COLOUR c_;

public:
    explicit Piece(PIECE_TYPE t, COLOUR c) : t_(t), c_(c)
    {
    }

    PIECE_TYPE getPieceType() const
    {
        return t_;
    }

    COLOUR getColour() const
    {
        return c_;
    }

    void setPieceType(PIECE_TYPE type)
    {
        t_ = type;
    }
};
