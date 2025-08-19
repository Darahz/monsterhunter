#pragma once

#include <SFML/Graphics.hpp>
#include <functional>

class Button {
public:
    Button(const std::string& text, const sf::Font& font, unsigned int characterSize);
    void setPosition(float x, float y);
    void setSize(float width, float height);
    void setCallback(std::function<void()> callback);
    void setBackgroundColor(const sf::Color& color);
    void setTextColor(const sf::Color& color);
    void render(sf::RenderTarget& target);
    bool isMouseOver(const sf::Vector2f& mousePos) const;
    bool handleEvent(const sf::Event& event);

private:
    sf::RectangleShape shape;
    sf::Text label;
    std::function<void()> onClick;
};
