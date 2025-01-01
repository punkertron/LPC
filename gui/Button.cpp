#include "Button.hpp"

Button::Button(sf::RenderWindow& window, int rowPos, const sf::Font& font, const char* text, const sf::Color& buttonColor,
               const sf::Color& textColor) :
    window_{window}
{
    text_.setFont(font);
    text_.setString(text);
    text_.setFillColor(textColor);

    auto floatRect = text_.getGlobalBounds();
    text_.setPosition((600 - floatRect.width) / 2, rowPos);

    const int delta = 10;
    shape_.setPosition((600 - text_.getGlobalBounds().width) / 2 - delta, rowPos - delta);
    shape_.setSize({floatRect.width + floatRect.left + 2 * delta, floatRect.height + floatRect.top + 2 * delta});
    shape_.setFillColor(buttonColor);
}

void Button::draw()
{
    window_.draw(shape_);
    window_.draw(text_);
}

void Button::setCallback(std::function<void()> func)
{
    callback_ = func;
}

void Button::handleEvent(const sf::Event& event)
{
    if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
        if (isMouseOver()) {
            // if (callback_)
            callback_();
        }
    }
}

bool Button::isMouseOver() const
{
    sf::Vector2i mousePos = sf::Mouse::getPosition(window_);
    sf::Vector2f buttonPos = shape_.getPosition();
    sf::Vector2f buttonSize = shape_.getSize();

    return (mousePos.x >= buttonPos.x && mousePos.x <= buttonPos.x + buttonSize.x && mousePos.y >= buttonPos.y &&
            mousePos.y <= buttonPos.y + buttonSize.y);
}