#pragma once

#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>

enum class COLOUR;

class ResourceManager final {
private:
    sf::Texture queen_;
    sf::Texture youLost_;
    sf::Texture youWin_;
    sf::Texture blackWins_;
    sf::Texture whiteWins_;
    sf::SoundBuffer winSound_;
    sf::SoundBuffer lostSound_;
    sf::Font font_;

public:
    ResourceManager();

    const sf::Texture& getQueenTexture() const;
    const sf::Texture& getYouLostTexture() const;
    const sf::Texture& getYouWinTexture() const;
    const sf::Texture& getColourWinsTexture(COLOUR c) const;
    const sf::SoundBuffer& getWinSoundBuffer() const;
    const sf::SoundBuffer& getLostSoundBuffer() const;
    const sf::Font& getFont() const;
};
