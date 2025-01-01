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
    sf::Font font_;

public:
    ResourceManager();

    const sf::Texture& getQueenTexture() const;
    const sf::Texture& getYouLostTexture() const;
    const sf::Texture& getYouWinTexture() const;
    const sf::Texture& getColourWinsTexture(COLOUR c) const;
    const sf::Font& getFont() const;
};
