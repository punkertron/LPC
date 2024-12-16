#include <cstdio>

#include "Board.hpp"

void printMoves(const std::vector<Move>& moves)
{
    for (const auto& m : moves) {
        std::printf("{%d %d %d %d}", m.from.row, m.from.col, m.to.row, m.to.col);
        auto nextMove = m.nextMove;
        while (nextMove) {
            std::printf(" {%d %d %d %d}", nextMove->from.row, nextMove->from.col, nextMove->to.row, nextMove->to.col);
            nextMove = nextMove->nextMove;
        }
        std::printf("\n");
    }
}

int main()
{
    Board b;
    b.printBoard();

    auto r = b.generateValidMoves({5, 1});

    b.makeMove({2, 0, 3, 1});
    b.makeMove({2, 2, 3, 3});
    b.makeMove({2, 6, 3, 5});
    b.makeMove({5, 1, 4, 0});

    b.printBoard();
    r = b.generateValidMoves({4, 0});
    printMoves(r);

    return 0;
}