#pragma once

#include <array>
#include <cstdint>

#include "Piece.hpp"

struct Position;

constexpr int MAX_BOARD_WIDTH = 72;

enum class BOARD_TYPE : uint8_t {
    EIGHTxEIGHT = 8,
    TENxTEN = 10,
    TWELVExTWELVE = 12,
};

struct Board {
private:
    std::array<Piece, MAX_BOARD_WIDTH> board_;
    BOARD_TYPE boardType_{BOARD_TYPE::EIGHTxEIGHT};

public:
    Piece& operator()(int row, int col);
    Piece operator()(int row, int col) const;
    Piece& operator()(Position pos);
    Piece operator()(Position pos) const;
    void setBoardType(BOARD_TYPE bt);
    void reset();
    int getWidth() const;
    BOARD_TYPE getBoardType() const;
};
