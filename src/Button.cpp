#include "../include/Button.h"
#include <algorithm>

Button::Button(const std::string& text, const sf::Font& font, unsigned int characterSize) 
    : normalColor(sf::Color(100, 100, 100, 200)), hoverColor(sf::Color(150, 150, 150, 200)), isHovered(false) {
    label.setFont(font);
    label.setString(text);
    label.setCharacterSize(characterSize);
    label.setFillColor(sf::Color::White);  // Default white text

    shape.setSize(sf::Vector2f(200, 50));
    shape.setFillColor(normalColor);  // Default gray background
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
    normalColor = color;
    hoverColor = sf::Color(
        std::min(255, static_cast<int>(color.r) + 50),
        std::min(255, static_cast<int>(color.g) + 50),
        std::min(255, static_cast<int>(color.b) + 50),
        color.a
    );
    shape.setFillColor(isHovered ? hoverColor : normalColor);
}

void Button::setTextColor(const sf::Color& color) {
    label.setFillColor(color);
}

void Button::updateHover(const sf::Vector2f& mousePos) {
    bool wasHovered = isHovered;
    isHovered = shape.getGlobalBounds().contains(mousePos);
    
    if (wasHovered != isHovered) {
        shape.setFillColor(isHovered ? hoverColor : normalColor);
    }
}

void Button::render(sf::RenderTarget& target) {
    target.draw(shape);
    target.draw(label);
}

bool Button::isMouseOver(const sf::Vector2f& mousePos) const {
    return shape.getGlobalBounds().contains(mousePos);
}

bool Button::handleEvent(const sf::Event& event) {
    if (event.type == sf::Event::MouseMoved) {
        updateHover(sf::Vector2f(event.mouseMove.x, event.mouseMove.y));
        return isHovered;
    }
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
