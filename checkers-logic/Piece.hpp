#pragma once

#include <cstdint>

enum class COLOUR;

class Piece {
private:
    enum class PIECE_TYPE : uint8_t {
        EMPTY = 0,
        WHITE_REGULAR,
        BLACK_REGULAR,
        WHITE_QUEEN,
        BLACK_QUEEN,
        CAPTURED,
    };

    PIECE_TYPE pt_;

public:
    bool isEmpty() const;
    bool isNotEmpty() const;
    COLOUR getColour() const;
    void setEmpty();
    void setBlackRegular();
    void setWhiteRegular();
    bool isRegular() const;
    bool isQueen() const;
    bool isCaptured() const;
    void setCaptured();
    void promoteToQueen();
};
