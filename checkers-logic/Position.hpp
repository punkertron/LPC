#pragma once

#include <cstddef>
#include <functional>

struct Position {
    int row;
    int col;

    friend auto operator<=>(const Position& left, const Position& right) = default;

    bool isValid() const
    {
        return row >= 0 && col >= 0;
    }

    void reset()
    {
        row = -1;
        col = -1;
    }
};

namespace std
{
template <>
struct hash<Position> {
    size_t operator()(const Position& pos) const
    {
        return hash<int>()(pos.row) ^ (hash<int>()(pos.col) << 1);
    }
};
}  // namespace std