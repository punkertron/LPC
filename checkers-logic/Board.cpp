#include "Board.hpp"

#include <algorithm>
#include <cassert>

#include "Position.hpp"

Piece& Board::operator()(int row, int col)
{
    assert((boardType_ == BOARD_TYPE::EIGHTxEIGHT ? row * static_cast<int>(boardType_) + col < 64
            : boardType_ == BOARD_TYPE::TENxTEN   ? row * static_cast<int>(boardType_) + col < 100
                                                  : row * static_cast<int>(boardType_) + col < 144));
    return board_[(row * static_cast<int>(boardType_) + col) / 2];
}

Piece Board::operator()(int row, int col) const
{
    assert((boardType_ == BOARD_TYPE::EIGHTxEIGHT ? row * static_cast<int>(boardType_) + col < 64
            : boardType_ == BOARD_TYPE::TENxTEN   ? row * static_cast<int>(boardType_) + col < 100
                                                  : row * static_cast<int>(boardType_) + col < 144));
    return board_[(row * static_cast<int>(boardType_) + col) / 2];
}

Piece& Board::operator()(Position pos)
{
    assert(pos.isValid());
    return operator()(pos.row, pos.col);
}

Piece Board::operator()(Position pos) const
{
    assert(pos.isValid());
    return operator()(pos.row, pos.col);
}

void Board::setBoardType(BOARD_TYPE bt)
{
    assert((bt == boardType_ || std::ranges::all_of(board_, [](Piece p) {
                return p.isEmpty();
            })));
    boardType_ = bt;
}

void Board::reset()
{
    std::ranges::for_each(board_, [](Piece& p) {
        p.setEmpty();
    });
}

int Board::getWidth() const
{
    return static_cast<int>(boardType_);
}

BOARD_TYPE Board::getBoardType() const
{
    return boardType_;
}
