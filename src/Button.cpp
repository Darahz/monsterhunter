#include "../include/Button.h"

Button::Button(const std::string& text, const sf::Font& font, unsigned int characterSize) {
    label.setFont(font);
    label.setString(text);
    label.setCharacterSize(characterSize);
    label.setFillColor(sf::Color::White);  // Default white text

    shape.setSize(sf::Vector2f(200, 50));
    shape.setFillColor(sf::Color(100, 100, 100, 25));  // Default gray background
}

void Button::setPosition(float x, float y) {
    shape.setPosition(x, y);
    label.setPosition(x + 10, y + 10);
}

void Button::setSize(float width, float height) {
    shape.setSize(sf::Vector2f(width, height));
}

void Button::setCallback(std::function<void()> callback) {
    onClick = callback;
}

void Button::setBackgroundColor(const sf::Color& color) {
    shape.setFillColor(color);
}

void Button::setTextColor(const sf::Color& color) {
    label.setFillColor(color);
}

void Button::render(sf::RenderTarget& target) {
    target.draw(shape);
    target.draw(label);
}

bool Button::isMouseOver(const sf::Vector2f& mousePos) const {
    return shape.getGlobalBounds().contains(mousePos);
}

bool Button::handleEvent(const sf::Event& event) {
    if (event.type == sf::Event::MouseButtonPressed) {
        if (event.mouseButton.button == sf::Mouse::Left) {
            sf::Vector2f mousePos(event.mouseButton.x, event.mouseButton.y);
            if (isMouseOver(mousePos)) {
                if (onClick) {
                    onClick();
                }
                return true;
            }
        }
    }
    return false;
}
