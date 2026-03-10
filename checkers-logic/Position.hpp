#pragma once

#include <cstddef>
#include <functional>

struct Position {
    int row{-1};
    int col{-1};

    friend auto operator<=>(const Position& left, const Position& right) = default;

    [[nodiscard]] bool isValid() const
    {
        return row >= 0 && col >= 0;
    }

    void reset()
    {
        row = -1;
        col = -1;
    }
};

template <>
struct std::hash<Position> {
    size_t operator()(const Position& pos) const noexcept
    {
        return hash<int>()(pos.row) ^ (hash<int>()(pos.col) << 1);
    }
};  // namespace std
