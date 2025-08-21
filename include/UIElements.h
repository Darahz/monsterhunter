#pragma once

#include <SFML/Graphics.hpp>
#include <functional>
#include <algorithm>

namespace UI {

class Button {
public:
    Button(const std::string& text, const sf::Font& font, unsigned int characterSize);
    void setPosition(float x, float y);
    void setSize(float width, float height);
    void setCallback(std::function<void()> callback);
    void setBackgroundColor(const sf::Color& color);
    void setTextColor(const sf::Color& color);
    void updateHover(const sf::Vector2f& mousePos);
    void render(sf::RenderTarget& target);
    bool isMouseOver(const sf::Vector2f& mousePos) const;
    bool handleEvent(const sf::Event& event);

private:
    sf::RectangleShape shape;
    sf::Text label;
    std::function<void()> onClick;
    sf::Color normalColor;
    sf::Color hoverColor;
    bool isHovered;
};

class Slider {
public:
    Slider(float minValue, float maxValue, float initialValue, const sf::Font& font);
    void setPosition(float x, float y);
    void setSize(float width, float height);
    void setValue(float value);
    float getValue() const;
    void setCallback(std::function<void(float)> callback);
    void setLabel(const std::string& labelText);
    void updateHover(const sf::Vector2f& mousePos);
    void render(sf::RenderTarget& target);
    bool handleEvent(const sf::Event& event);

private:
    sf::RectangleShape track;
    sf::RectangleShape handle;
    sf::Text label;
    sf::Text valueText;
    
    float minValue;
    float maxValue;
    float currentValue;
    float trackWidth;
    float trackHeight;
    float handleWidth;
    float handleHeight;
    sf::Vector2f position;
    
    bool isDragging;
    bool isHovered;
    sf::Color trackColor;
    sf::Color handleColor;
    sf::Color handleHoverColor;
    
    std::function<void(float)> onValueChanged;
    
    void updateHandlePosition();
    void updateValueFromMouseX(float mouseX);
    std::string formatValue() const;
};

class Label {
public:
    Label(const std::string& text, const sf::Font& font, unsigned int characterSize = 16);
    void setPosition(float x, float y);
    void setText(const std::string& text);
    void setColor(const sf::Color& color);
    void setCharacterSize(unsigned int size);
    void render(sf::RenderTarget& target);
    sf::Vector2f getSize() const;

private:
    sf::Text text;
};

class Checkbox {
public:
    Checkbox(const std::string& labelText, const sf::Font& font, bool initialValue = false);
    void setPosition(float x, float y);
    void setChecked(bool checked);
    bool isChecked() const;
    void setCallback(std::function<void(bool)> callback);
    void updateHover(const sf::Vector2f& mousePos);
    void render(sf::RenderTarget& target);
    bool handleEvent(const sf::Event& event);

private:
    sf::RectangleShape checkBox;
    sf::RectangleShape checkMark;
    sf::Text label;
    bool checked;
    bool isHovered;
    sf::Color boxColor;
    sf::Color boxHoverColor;
    sf::Color checkColor;
    sf::Vector2f position;
    std::function<void(bool)> onValueChanged;
};

class Dropdown {
public:
    Dropdown(const sf::Font& font, float width = 200.0f);
    void setPosition(float x, float y);
    void addItem(const std::string& item);
    void setSelectedIndex(int index);
    int getSelectedIndex() const;
    std::string getSelectedItem() const;
    void setCallback(std::function<void(int, const std::string&)> callback);
    void updateHover(const sf::Vector2f& mousePos);
    void render(sf::RenderTarget& target);
    bool handleEvent(const sf::Event& event);

private:
    sf::RectangleShape mainBox;
    sf::Text selectedText;
    sf::RectangleShape dropdownArrow;
    std::vector<sf::RectangleShape> itemBoxes;
    std::vector<sf::Text> itemTexts;
    std::vector<std::string> items;
    
    const sf::Font* font;
    float width;
    float itemHeight;
    sf::Vector2f position;
    int selectedIndex;
    bool isOpen;
    bool isHovered;
    int hoveredItemIndex;
    
    sf::Color mainBoxColor;
    sf::Color mainBoxHoverColor;
    sf::Color itemBoxColor;
    sf::Color itemBoxHoverColor;
    sf::Color textColor;
    
    std::function<void(int, const std::string&)> onSelectionChanged;
    
    void updateItemPositions();
    void closeDropdown();
};

}