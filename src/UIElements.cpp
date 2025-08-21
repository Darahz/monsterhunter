#include "../include/UIElements.h"
#include <sstream>
#include <iomanip>

namespace UI {

// Button Implementation
Button::Button(const std::string& text, const sf::Font& font, unsigned int characterSize) 
    : normalColor(sf::Color(100, 100, 100, 200)), hoverColor(sf::Color(150, 150, 150, 200)), isHovered(false) {
    label.setFont(font);
    label.setString(text);
    label.setCharacterSize(characterSize);
    label.setFillColor(sf::Color::White);

    shape.setSize(sf::Vector2f(200, 50));
    shape.setFillColor(normalColor);
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

// Slider Implementation
Slider::Slider(float minValue, float maxValue, float initialValue, const sf::Font& font)
    : minValue(minValue), maxValue(maxValue), currentValue(initialValue),
      trackWidth(200.0f), trackHeight(10.0f), handleWidth(20.0f), handleHeight(20.0f),
      isDragging(false), isHovered(false),
      trackColor(sf::Color(80, 80, 80)), handleColor(sf::Color(150, 150, 150)),
      handleHoverColor(sf::Color(200, 200, 200)) {
    
    track.setSize(sf::Vector2f(trackWidth, trackHeight));
    track.setFillColor(trackColor);
    
    handle.setSize(sf::Vector2f(handleWidth, handleHeight));
    handle.setFillColor(handleColor);
    
    label.setFont(font);
    label.setCharacterSize(16);
    label.setFillColor(sf::Color::White);
    
    valueText.setFont(font);
    valueText.setCharacterSize(14);
    valueText.setFillColor(sf::Color::White);
    valueText.setString(formatValue());
}

void Slider::setPosition(float x, float y) {
    position = sf::Vector2f(x, y);
    track.setPosition(x, y + 20);
    
    if (!label.getString().isEmpty()) {
        label.setPosition(x, y);
    }
    
    valueText.setPosition(x + trackWidth + 10, y + 15);
    updateHandlePosition();
}

void Slider::setSize(float width, float height) {
    trackWidth = width;
    trackHeight = height;
    track.setSize(sf::Vector2f(trackWidth, trackHeight));
    updateHandlePosition();
}

void Slider::setValue(float value) {
    currentValue = std::clamp(value, minValue, maxValue);
    valueText.setString(formatValue());
    updateHandlePosition();
    if (onValueChanged) {
        onValueChanged(currentValue);
    }
}

float Slider::getValue() const {
    return currentValue;
}

void Slider::setCallback(std::function<void(float)> callback) {
    onValueChanged = callback;
}

void Slider::setLabel(const std::string& labelText) {
    label.setString(labelText);
}

void Slider::updateHover(const sf::Vector2f& mousePos) {
    sf::FloatRect handleBounds = handle.getGlobalBounds();
    bool wasHovered = isHovered;
    isHovered = handleBounds.contains(mousePos);
    
    if (wasHovered != isHovered) {
        handle.setFillColor(isHovered ? handleHoverColor : handleColor);
    }
}

void Slider::render(sf::RenderTarget& target) {
    target.draw(track);
    target.draw(handle);
    
    if (!label.getString().isEmpty()) {
        target.draw(label);
    }
    
    target.draw(valueText);
}

bool Slider::handleEvent(const sf::Event& event) {
    if (event.type == sf::Event::MouseMoved) {
        updateHover(sf::Vector2f(event.mouseMove.x, event.mouseMove.y));
        
        if (isDragging) {
            updateValueFromMouseX(event.mouseMove.x);
            return true;
        }
    }
    
    if (event.type == sf::Event::MouseButtonPressed) {
        if (event.mouseButton.button == sf::Mouse::Left) {
            sf::Vector2f mousePos(event.mouseButton.x, event.mouseButton.y);
            sf::FloatRect trackBounds = track.getGlobalBounds();
            sf::FloatRect handleBounds = handle.getGlobalBounds();
            
            if (handleBounds.contains(mousePos) || trackBounds.contains(mousePos)) {
                isDragging = true;
                updateValueFromMouseX(event.mouseButton.x);
                return true;
            }
        }
    }
    
    if (event.type == sf::Event::MouseButtonReleased) {
        if (event.mouseButton.button == sf::Mouse::Left && isDragging) {
            isDragging = false;
            return true;
        }
    }
    
    return false;
}

void Slider::updateHandlePosition() {
    float ratio = (currentValue - minValue) / (maxValue - minValue);
    float handleX = position.x + (ratio * (trackWidth - handleWidth));
    float handleY = position.y + 20 - (handleHeight - trackHeight) / 2;
    handle.setPosition(handleX, handleY);
}

void Slider::updateValueFromMouseX(float mouseX) {
    float trackLeft = track.getPosition().x;
    float trackRight = trackLeft + trackWidth - handleWidth;
    float clampedX = std::clamp(mouseX - handleWidth / 2, trackLeft, trackRight);
    
    float ratio = (clampedX - trackLeft) / (trackRight - trackLeft);
    float newValue = minValue + ratio * (maxValue - minValue);
    
    setValue(newValue);
}

std::string Slider::formatValue() const {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(1) << currentValue;
    return oss.str();
}

// Label Implementation
Label::Label(const std::string& labelText, const sf::Font& font, unsigned int characterSize) {
    text.setFont(font);
    text.setString(labelText);
    text.setCharacterSize(characterSize);
    text.setFillColor(sf::Color::White);
}

void Label::setPosition(float x, float y) {
    text.setPosition(x, y);
}

void Label::setText(const std::string& labelText) {
    text.setString(labelText);
}

void Label::setColor(const sf::Color& color) {
    text.setFillColor(color);
}

void Label::setCharacterSize(unsigned int size) {
    text.setCharacterSize(size);
}

void Label::render(sf::RenderTarget& target) {
    target.draw(text);
}

sf::Vector2f Label::getSize() const {
    sf::FloatRect bounds = text.getLocalBounds();
    return sf::Vector2f(bounds.width, bounds.height);
}

// Checkbox Implementation
Checkbox::Checkbox(const std::string& labelText, const sf::Font& font, bool initialValue)
    : checked(initialValue), isHovered(false),
      boxColor(sf::Color(80, 80, 80)), boxHoverColor(sf::Color(120, 120, 120)),
      checkColor(sf::Color::White) {
    
    checkBox.setSize(sf::Vector2f(20, 20));
    checkBox.setFillColor(boxColor);
    checkBox.setOutlineThickness(2);
    checkBox.setOutlineColor(sf::Color::White);
    
    checkMark.setSize(sf::Vector2f(12, 12));
    checkMark.setFillColor(checkColor);
    
    label.setFont(font);
    label.setString(labelText);
    label.setCharacterSize(16);
    label.setFillColor(sf::Color::White);
}

void Checkbox::setPosition(float x, float y) {
    position = sf::Vector2f(x, y);
    checkBox.setPosition(x, y);
    checkMark.setPosition(x + 4, y + 4);
    label.setPosition(x + 30, y);
}

void Checkbox::setChecked(bool isChecked) {
    checked = isChecked;
    if (onValueChanged) {
        onValueChanged(checked);
    }
}

bool Checkbox::isChecked() const {
    return checked;
}

void Checkbox::setCallback(std::function<void(bool)> callback) {
    onValueChanged = callback;
}

void Checkbox::updateHover(const sf::Vector2f& mousePos) {
    bool wasHovered = isHovered;
    isHovered = checkBox.getGlobalBounds().contains(mousePos);
    
    if (wasHovered != isHovered) {
        checkBox.setFillColor(isHovered ? boxHoverColor : boxColor);
    }
}

void Checkbox::render(sf::RenderTarget& target) {
    target.draw(checkBox);
    if (checked) {
        target.draw(checkMark);
    }
    target.draw(label);
}

bool Checkbox::handleEvent(const sf::Event& event) {
    if (event.type == sf::Event::MouseMoved) {
        updateHover(sf::Vector2f(event.mouseMove.x, event.mouseMove.y));
        return isHovered;
    }
    
    if (event.type == sf::Event::MouseButtonPressed) {
        if (event.mouseButton.button == sf::Mouse::Left) {
            sf::Vector2f mousePos(event.mouseButton.x, event.mouseButton.y);
            if (checkBox.getGlobalBounds().contains(mousePos)) {
                checked = !checked;
                if (onValueChanged) {
                    onValueChanged(checked);
                }
                return true;
            }
        }
    }
    
    return false;
}

// Dropdown Implementation
Dropdown::Dropdown(const sf::Font& font, float width)
    : font(&font), width(width), itemHeight(30.0f), selectedIndex(-1),
      isOpen(false), isHovered(false), hoveredItemIndex(-1),
      mainBoxColor(sf::Color(80, 80, 80)), mainBoxHoverColor(sf::Color(120, 120, 120)),
      itemBoxColor(sf::Color(60, 60, 60)), itemBoxHoverColor(sf::Color(100, 100, 100)),
      textColor(sf::Color::White) {
    
    mainBox.setSize(sf::Vector2f(width, itemHeight));
    mainBox.setFillColor(mainBoxColor);
    mainBox.setOutlineThickness(2);
    mainBox.setOutlineColor(sf::Color::White);
    
    selectedText.setFont(font);
    selectedText.setCharacterSize(16);
    selectedText.setFillColor(textColor);
    selectedText.setString("Select...");
    
    dropdownArrow.setSize(sf::Vector2f(10, 6));
    dropdownArrow.setFillColor(textColor);
}

void Dropdown::setPosition(float x, float y) {
    position = sf::Vector2f(x, y);
    mainBox.setPosition(x, y);
    selectedText.setPosition(x + 10, y + 5);
    dropdownArrow.setPosition(x + width - 20, y + 12);
    updateItemPositions();
}

void Dropdown::addItem(const std::string& item) {
    items.push_back(item);
    
    sf::RectangleShape itemBox;
    itemBox.setSize(sf::Vector2f(width, itemHeight));
    itemBox.setFillColor(itemBoxColor);
    itemBox.setOutlineThickness(1);
    itemBox.setOutlineColor(sf::Color(100, 100, 100));
    itemBoxes.push_back(itemBox);
    
    sf::Text itemText;
    itemText.setFont(*font);
    itemText.setString(item);
    itemText.setCharacterSize(16);
    itemText.setFillColor(textColor);
    itemTexts.push_back(itemText);
    
    updateItemPositions();
}

void Dropdown::setSelectedIndex(int index) {
    if (index >= 0 && index < static_cast<int>(items.size())) {
        selectedIndex = index;
        selectedText.setString(items[index]);
        if (onSelectionChanged) {
            onSelectionChanged(index, items[index]);
        }
    }
}

int Dropdown::getSelectedIndex() const {
    return selectedIndex;
}

std::string Dropdown::getSelectedItem() const {
    if (selectedIndex >= 0 && selectedIndex < static_cast<int>(items.size())) {
        return items[selectedIndex];
    }
    return "";
}

void Dropdown::setCallback(std::function<void(int, const std::string&)> callback) {
    onSelectionChanged = callback;
}

void Dropdown::updateHover(const sf::Vector2f& mousePos) {
    bool wasHovered = isHovered;
    isHovered = mainBox.getGlobalBounds().contains(mousePos);
    
    if (wasHovered != isHovered) {
        mainBox.setFillColor(isHovered ? mainBoxHoverColor : mainBoxColor);
    }
    
    if (isOpen) {
        int oldHoveredIndex = hoveredItemIndex;
        hoveredItemIndex = -1;
        
        for (int i = 0; i < static_cast<int>(itemBoxes.size()); ++i) {
            if (itemBoxes[i].getGlobalBounds().contains(mousePos)) {
                hoveredItemIndex = i;
                break;
            }
        }
        
        if (oldHoveredIndex != hoveredItemIndex) {
            if (oldHoveredIndex >= 0) {
                itemBoxes[oldHoveredIndex].setFillColor(itemBoxColor);
            }
            if (hoveredItemIndex >= 0) {
                itemBoxes[hoveredItemIndex].setFillColor(itemBoxHoverColor);
            }
        }
    }
}

void Dropdown::render(sf::RenderTarget& target) {
    target.draw(mainBox);
    target.draw(selectedText);
    target.draw(dropdownArrow);
    
    if (isOpen) {
        for (const auto& itemBox : itemBoxes) {
            target.draw(itemBox);
        }
        for (const auto& itemText : itemTexts) {
            target.draw(itemText);
        }
    }
}

bool Dropdown::handleEvent(const sf::Event& event) {
    if (event.type == sf::Event::MouseMoved) {
        updateHover(sf::Vector2f(event.mouseMove.x, event.mouseMove.y));
        return isHovered || isOpen;
    }
    
    if (event.type == sf::Event::MouseButtonPressed) {
        if (event.mouseButton.button == sf::Mouse::Left) {
            sf::Vector2f mousePos(event.mouseButton.x, event.mouseButton.y);
            
            if (mainBox.getGlobalBounds().contains(mousePos)) {
                isOpen = !isOpen;
                return true;
            }
            
            if (isOpen) {
                for (int i = 0; i < static_cast<int>(itemBoxes.size()); ++i) {
                    if (itemBoxes[i].getGlobalBounds().contains(mousePos)) {
                        setSelectedIndex(i);
                        closeDropdown();
                        return true;
                    }
                }
                closeDropdown();
            }
        }
    }
    
    return false;
}

void Dropdown::updateItemPositions() {
    for (int i = 0; i < static_cast<int>(itemBoxes.size()); ++i) {
        float y = position.y + itemHeight * (i + 1);
        itemBoxes[i].setPosition(position.x, y);
        itemTexts[i].setPosition(position.x + 10, y + 5);
    }
}

void Dropdown::closeDropdown() {
    isOpen = false;
    hoveredItemIndex = -1;
    for (auto& itemBox : itemBoxes) {
        itemBox.setFillColor(itemBoxColor);
    }
}

}