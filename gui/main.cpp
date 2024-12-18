#include <SFML/Graphics.hpp>

#include "Board.hpp"
#include "Game.hpp"

int main()
{
    Game game;
    game.run();

    // Board b;
    // b.printBoard();
    // auto f = b.getBoard();
    // b.makeMove({2, 0, 3, 1});
    // b.makeMove({2, 2, 3, 3});
    // b.makeMove({2, 6, 3, 5});
    // b.makeMove({5, 1, 4, 0});
    // b.printBoard();
    // auto moves = b.generateValidMoves({4, 0});
    // b.makeMove(moves[0]);
    // b.printBoard();
    return 0;
}