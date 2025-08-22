#pragma once
#include "Screen.h"
#include "UIElements.h"
#include <string>

class NewGameScreen : public Screen {
public:
    NewGameScreen(sf::Vector2u windowSize, const sf::Font& font);
    void update(float deltaTime) override;
    void render(sf::RenderWindow& window) override;
    void handleEvent(const sf::Event& event) override;
    void onEnter() override;
    void updateWindowSize(sf::Vector2u newSize) override;

    // Getter for the entered player name
    std::string getPlayerName() const;

private:
    void setupUI(const sf::Font& font);
    void startGame();
    
    // UI elements
    std::vector<std::unique_ptr<UI::Label>> labels;
    std::vector<std::unique_ptr<UI::Button>> buttons;
    
    // Text input handling
    sf::RectangleShape inputBox;
    sf::Text inputText;
    sf::Text placeholderText;
    std::string playerName;
    bool isInputActive;
    bool isInputHovered;
    
    // Visual properties
    sf::Color inputBoxColor;
    sf::Color inputBoxActiveColor;
    sf::Color inputBoxHoverColor;
    sf::Color textColor;
    sf::Color placeholderColor;
    
    const sf::Font* storedFont;
    static const size_t MAX_NAME_LENGTH = 20;
};
