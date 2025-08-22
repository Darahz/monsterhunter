#include "../include/NewGame.h"
#include <iostream>

NewGameScreen::NewGameScreen(sf::Vector2u windowSize, const sf::Font& font)
    : Screen(ScreenType::NewGame, windowSize), playerName(""), isInputActive(false), isInputHovered(false), storedFont(&font) {
    
    // Set up colors
    inputBoxColor = sf::Color(50, 50, 50, 200);
    inputBoxActiveColor = sf::Color(70, 70, 70, 220);
    inputBoxHoverColor = sf::Color(60, 60, 60, 210);
    textColor = sf::Color::White;
    placeholderColor = sf::Color(150, 150, 150);
    
    setupUI(font);
    weather->setWeatherType(WeatherType::None);
    weather->startUpdateThread();
}

void NewGameScreen::setupUI(const sf::Font& font) {
    // Clear existing UI elements
    labels.clear();
    buttons.clear();
    
    float centerX = static_cast<float>(windowSize.x) / 2.0f;
    float centerY = static_cast<float>(windowSize.y) / 2.0f;
    
    // Title label
    auto titleLabel = std::make_unique<UI::Label>("New Game", font, 36);
    titleLabel->setPosition(centerX - 80, centerY - 150);
    titleLabel->setColor(sf::Color::White);
    labels.push_back(std::move(titleLabel));
    
    // Instructions label
    auto instructionLabel = std::make_unique<UI::Label>("Enter your hunter name:", font, 20);
    instructionLabel->setPosition(centerX - 120, centerY - 80);
    instructionLabel->setColor(sf::Color::White);
    labels.push_back(std::move(instructionLabel));
    
    // Set up input box
    float inputWidth = 300.0f;
    float inputHeight = 40.0f;
    inputBox.setSize(sf::Vector2f(inputWidth, inputHeight));
    inputBox.setPosition(centerX - inputWidth / 2, centerY - 20);
    inputBox.setFillColor(inputBoxColor);
    inputBox.setOutlineThickness(2.0f);
    inputBox.setOutlineColor(sf::Color(100, 100, 100));
    
    // Set up input text
    inputText.setFont(font);
    inputText.setCharacterSize(18);
    inputText.setFillColor(textColor);
    inputText.setPosition(centerX - inputWidth / 2 + 10, centerY - 15);
    
    // Set up placeholder text
    placeholderText.setFont(font);
    placeholderText.setCharacterSize(18);
    placeholderText.setFillColor(placeholderColor);
    placeholderText.setString("Enter your name...");
    placeholderText.setPosition(centerX - inputWidth / 2 + 10, centerY - 15);
    
    // Start Game button
    auto startButton = std::make_unique<UI::Button>("Start Adventure", font, 24);
    startButton->setPosition(centerX - 150, centerY + 50);
    startButton->setSize(300, 55);
    startButton->setCallback([this]() {
        startGame();
    });
    buttons.push_back(std::move(startButton));
    
    // Back button
    auto backButton = std::make_unique<UI::Button>("Back", font, 24);
    backButton->setPosition(centerX - 150, centerY + 130);
    backButton->setSize(300, 55);
    backButton->setCallback([this]() {
        if (onScreenChange) {
            onScreenChange(ScreenType::MainMenu);
        }
    });
    buttons.push_back(std::move(backButton));
}

void NewGameScreen::update(float deltaTime) {
    Screen::update(deltaTime);
    
    // Update UI elements - buttons don't need explicit updates in our current implementation
    // but we keep this structure for potential future button animation/hover effects
    (void)deltaTime; // Suppress unused parameter warning
}

void NewGameScreen::render(sf::RenderWindow& window) {
    Screen::render(window);
    
    // Render labels
    for (const auto& label : labels) {
        label->render(window);
    }
    
    // Render input box
    window.draw(inputBox);
    
    // Render text (either input text or placeholder)
    if (playerName.empty() && !isInputActive) {
        window.draw(placeholderText);
    } else {
        window.draw(inputText);
    }
    
    // Render buttons
    for (const auto& button : buttons) {
        button->render(window);
    }
}

void NewGameScreen::handleEvent(const sf::Event& event) {
    Screen::handleEvent(event);
    
    // Handle button events
    for (auto& button : buttons) {
        button->handleEvent(event);
    }
    
    // Handle mouse events for input box
    if (event.type == sf::Event::MouseMoved) {
        sf::Vector2f mousePos(static_cast<float>(event.mouseMove.x), static_cast<float>(event.mouseMove.y));
        isInputHovered = inputBox.getGlobalBounds().contains(mousePos);
        
        // Update input box color based on state
        if (isInputActive) {
            inputBox.setFillColor(inputBoxActiveColor);
        } else if (isInputHovered) {
            inputBox.setFillColor(inputBoxHoverColor);
        } else {
            inputBox.setFillColor(inputBoxColor);
        }
    }
    
    if (event.type == sf::Event::MouseButtonPressed) {
        if (event.mouseButton.button == sf::Mouse::Left) {
            sf::Vector2f mousePos(static_cast<float>(event.mouseButton.x), static_cast<float>(event.mouseButton.y));
            isInputActive = inputBox.getGlobalBounds().contains(mousePos);
            
            // Update input box color
            if (isInputActive) {
                inputBox.setFillColor(inputBoxActiveColor);
            } else {
                inputBox.setFillColor(isInputHovered ? inputBoxHoverColor : inputBoxColor);
            }
        }
    }
    
    // Handle text input
    if (isInputActive) {
        if (event.type == sf::Event::TextEntered) {
            if (event.text.unicode < 128) { // ASCII characters only
                char enteredChar = static_cast<char>(event.text.unicode);
                
                if (enteredChar == '\b') { // Backspace
                    if (!playerName.empty()) {
                        playerName.pop_back();
                        inputText.setString(playerName);
                    }
                } else if (enteredChar == '\r' || enteredChar == '\n') { // Enter
                    startGame();
                } else if (enteredChar >= 32 && playerName.length() < MAX_NAME_LENGTH) { // Printable characters
                    playerName += enteredChar;
                    inputText.setString(playerName);
                }
            }
        }
        
        if (event.type == sf::Event::KeyPressed) {
            if (event.key.code == sf::Keyboard::Escape) {
                isInputActive = false;
                inputBox.setFillColor(isInputHovered ? inputBoxHoverColor : inputBoxColor);
            }
        }
    }
}

void NewGameScreen::onEnter() {
    Screen::onEnter();
    // Reset input state
    playerName.clear();
    inputText.setString("");
    isInputActive = false;
    isInputHovered = false;
    inputBox.setFillColor(inputBoxColor);
}

void NewGameScreen::updateWindowSize(sf::Vector2u newSize) {
    Screen::updateWindowSize(newSize);
    
    // Recreate UI with new positions
    if (storedFont) {
        setupUI(*storedFont);
    }
}

void NewGameScreen::startGame() {
    if (playerName.empty()) {
        return;
    }
    
    // Trim whitespace
    std::string trimmedName = playerName;
    trimmedName.erase(0, trimmedName.find_first_not_of(" \t"));
    trimmedName.erase(trimmedName.find_last_not_of(" \t") + 1);
    
    if (trimmedName.empty()) {
        return;
    }
    
    playerName = trimmedName;
    
    if (onScreenChange) {
        onScreenChange(ScreenType::Game);
    }
}

std::string NewGameScreen::getPlayerName() const {
    return playerName;
}
