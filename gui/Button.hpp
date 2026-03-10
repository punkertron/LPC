#pragma once

#include <SFML/Graphics.hpp>
#include <functional>

class Button {
private:
    sf::RenderWindow& window_;
    sf::RectangleShape shape_;
    sf::Text text_;
    std::function<void()> callback_;
    float rowRatio_;
    sf::Vector2u lastLayoutWindowSize_{};

    bool isMouseOver() const;
    void updateLayout();

public:
    Button(sf::RenderWindow& window, float rowRatio, const sf::Font& font, const char* text,
           const sf::Color& buttonColor = sf::Color::Magenta, const sf::Color& textColor = sf::Color::White);

    void draw();
    void handleEvent(const sf::Event& event);
    void setCallback(std::function<void()> func);
};
