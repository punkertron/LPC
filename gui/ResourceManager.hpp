#pragma once

#include <SFML/Graphics.hpp>

#include "Piece.hpp"

class ResourceManager final {
private:
    sf::Texture queen_;
    sf::Texture youLost_;
    sf::Texture youWin_;
    sf::Texture blackWins_;
    sf::Texture whiteWins_;

public:
    ResourceManager();

    sf::Texture& getQueenTexture();
    sf::Texture& getYouLostTexture();
    sf::Texture& getYouWinTexture();
    sf::Texture& getColourWinsTexture(COLOUR c);
};
