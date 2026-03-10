#include "Button.hpp"

#include <algorithm>

namespace
{
constexpr unsigned int kMinButtonCharacterSize = 20;
constexpr unsigned int kMaxButtonCharacterSize = 42;
}  // namespace

Button::Button(sf::RenderWindow& window, float rowRatio, const sf::Font& font, const char* text,
               const sf::Color& buttonColor, const sf::Color& textColor) :
    window_{window}, text_{font, text}, rowRatio_{std::clamp(rowRatio, 0.f, 1.f)}
{
    text_.setFillColor(textColor);
    shape_.setFillColor(buttonColor);
    updateLayout();
}

void Button::draw()
{
    updateLayout();
    window_.draw(shape_);
    window_.draw(text_);
}

void Button::setCallback(std::function<void()> func)
{
    callback_ = func;
}

void Button::handleEvent(const sf::Event& event)
{
    updateLayout();
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

void Button::updateLayout()
{
    const sf::Vector2u windowSize = window_.getSize();
    if (windowSize == lastLayoutWindowSize_) {
        return;
    }

    lastLayoutWindowSize_ = windowSize;

    const auto referenceLength = static_cast<float>(std::min(windowSize.x, windowSize.y));
    auto characterSize = static_cast<unsigned int>(std::clamp(
        referenceLength * 0.05f, static_cast<float>(kMinButtonCharacterSize), static_cast<float>(kMaxButtonCharacterSize)));
    text_.setCharacterSize(characterSize);

    const float maxTextWidth = static_cast<float>(windowSize.x) * 0.78f;
    auto textBounds = text_.getLocalBounds();
    while (characterSize > 16 && textBounds.size.x > maxTextWidth) {
        --characterSize;
        text_.setCharacterSize(characterSize);
        textBounds = text_.getLocalBounds();
    }

    text_.setPosition({(static_cast<float>(windowSize.x) - textBounds.size.x) / 2.f - textBounds.position.x,
                       rowRatio_ * static_cast<float>(windowSize.y) - textBounds.position.y});

    textBounds = text_.getGlobalBounds();
    const float padding = std::max(10.f, static_cast<float>(characterSize) * 0.35f);
    shape_.setPosition({textBounds.position.x - padding, textBounds.position.y - padding});
    shape_.setSize({textBounds.size.x + 2.f * padding, textBounds.size.y + 2.f * padding});
}
