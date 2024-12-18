#include "PlayState.hpp"

constexpr int WIDTH = 600;
constexpr int BOARD_SIZE = 8;
constexpr int TILE = 600 / BOARD_SIZE;
constexpr float PIECE_RADIUS = (TILE - 20) / 2.0f;
const sf::Color LIGHT_COLOR = sf::Color(240, 217, 181);
const sf::Color DARK_COLOR = sf::Color(181, 136, 99);
const sf::Color LIGHT_PIECE = sf::Color(240, 240, 240);
const sf::Color DARK_PIECE = sf::Color(75, 75, 75);

PlayState::PlayState(sf::RenderWindow& window) : State{window}
{
    copyBoard_ = b.getBoard();
}

void PlayState::handleEvent(const sf::Event& event)
{
}

void PlayState::update()
{
}

void PlayState::render()
{
    for (int row = 0; row < BOARD_SIZE; ++row) {
        int realRow = 7 - row;
        for (int col = 0; col < BOARD_SIZE; ++col) {
            sf::RectangleShape square{
                sf::Vector2f{TILE, TILE}
            };

            square.setPosition(col * TILE, row * TILE);
            square.setFillColor((row + col) % 2 == 0 ? LIGHT_COLOR : DARK_COLOR);
            window_.draw(square);

            if (copyBoard_[realRow][col]) {
                sf::CircleShape piece{PIECE_RADIUS, 50};

                piece.setPosition(col * TILE + 10, row * TILE + 10);
                piece.setFillColor(copyBoard_[realRow][col]->getColour() == COLOUR::WHITE ? LIGHT_PIECE : DARK_PIECE);
                window_.draw(piece);
            }
        }
    }
}
