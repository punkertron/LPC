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

    PIECE_TYPE pt_{PIECE_TYPE::EMPTY};

public:
    [[nodiscard]] bool isEmpty() const;
    [[nodiscard]] bool isNotEmpty() const;
    [[nodiscard]] COLOUR getColour() const;
    void setEmpty();
    void setBlackRegular();
    void setWhiteRegular();
    [[nodiscard]] bool isRegular() const;
    [[nodiscard]] bool isQueen() const;
    [[nodiscard]] bool isCaptured() const;
    void setCaptured();
    void promoteToQueen();
};
