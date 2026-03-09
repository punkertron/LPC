#include "Button.hpp"

Button::Button(sf::RenderWindow& window, int rowPos, const sf::Font& font, const char* text, const sf::Color& buttonColor,
               const sf::Color& textColor) :
    window_{window}, text_{font, text}
{
    text_.setFillColor(textColor);

    const auto floatRect = text_.getGlobalBounds();
    text_.setPosition({(600.f - floatRect.size.x) / 2.f, static_cast<float>(rowPos)});

    const int delta = 10;
    shape_.setPosition({(600.f - text_.getGlobalBounds().size.x) / 2.f - delta, static_cast<float>(rowPos - delta)});
    shape_.setSize(
        {floatRect.size.x + floatRect.position.x + 2.f * delta, floatRect.size.y + floatRect.position.y + 2.f * delta});
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
    if (const auto* mousePressed = event.getIf<sf::Event::MouseButtonPressed>();
        mousePressed != nullptr && mousePressed->button == sf::Mouse::Button::Left) {
        if (isMouseOver() && callback_) {
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
