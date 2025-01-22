#include "Piece.hpp"

#include <cassert>
#include <stdexcept>

#include "Checkers.hpp"

bool Piece::isEmpty() const
{
    return pt_ == PIECE_TYPE::EMPTY;
}

bool Piece::isNotEmpty() const
{
    return !isEmpty();
}

COLOUR Piece::getColour() const
{
    assert(pt_ != PIECE_TYPE::EMPTY || pt_ != PIECE_TYPE::CAPTURED);
    switch (pt_) {
        case PIECE_TYPE::WHITE_REGULAR:
            return COLOUR::WHITE;
        case PIECE_TYPE::BLACK_REGULAR:
            return COLOUR::BLACK;
        case PIECE_TYPE::WHITE_QUEEN:
            return COLOUR::WHITE;
        case PIECE_TYPE::BLACK_QUEEN:
            return COLOUR::BLACK;
        default:
            throw std::logic_error("Attempt to get COLOUR from Empty or Captured Piece");
    }
}

void Piece::setEmpty()
{
    pt_ = Piece::PIECE_TYPE::EMPTY;
}

void Piece::setBlackRegular()
{
    pt_ = PIECE_TYPE::BLACK_REGULAR;
}

void Piece::Piece::setWhiteRegular()
{
    pt_ = PIECE_TYPE::WHITE_REGULAR;
}

bool Piece::isRegular() const
{
    return pt_ == PIECE_TYPE::WHITE_REGULAR || pt_ == PIECE_TYPE::BLACK_REGULAR;
}

bool Piece::isQueen() const
{
    return pt_ == PIECE_TYPE::WHITE_QUEEN || pt_ == PIECE_TYPE::BLACK_QUEEN;
}

bool Piece::isCaptured() const
{
    return pt_ == PIECE_TYPE::CAPTURED;
}

void Piece::setCaptured()
{
    pt_ = PIECE_TYPE::CAPTURED;
}

void Piece::promoteToQueen()
{
    assert(!(pt_ == PIECE_TYPE::WHITE_QUEEN || pt_ == PIECE_TYPE::BLACK_QUEEN));
    switch (pt_) {
        case PIECE_TYPE::WHITE_REGULAR:
            pt_ = PIECE_TYPE::WHITE_QUEEN;
            return;
        case PIECE_TYPE::BLACK_REGULAR:
            pt_ = PIECE_TYPE::BLACK_QUEEN;
            return;
        default:
            throw std::logic_error("Cannot promote Queen to Queen");
    }
}
