#include "../include/Screen.h"
#include <iostream>

// Base Screen class implementation
Screen::Screen(ScreenType type, sf::Vector2u windowSize) 
    : screenType(type), windowSize(windowSize), isActive(false) {
    weather = std::make_unique<Weather>();
    screenEffects = std::make_unique<ScreenEffects>();
    weather->setWindowSize(static_cast<float>(windowSize.x), static_cast<float>(windowSize.y));
}

void Screen::update(float deltaTime) {
    if (!isActive) return;
    
    if (weather) {
        weather->update(deltaTime);
    }
    if (screenEffects) {
        screenEffects->update(deltaTime);
    }
}

void Screen::render(sf::RenderWindow& window) {
    if (!isActive) return;

    // Draw weather effects
    if (weather) {
        weather->draw(window);
    }

    // Draw buttons
    for (auto& button : buttons) {
        button->render(window);
    }

    // Draw screen effects on top
    if (screenEffects) {
        screenEffects->fadeIn(window);
    }
}

void Screen::handleEvent(const sf::Event& event) {
    if (!isActive) return;

    // Handle button events
    for (auto& button : buttons) {
        button->handleEvent(event);
    }

    // Update button hover states on mouse move
    if (event.type == sf::Event::MouseMoved) {
        sf::Vector2f mousePos(static_cast<float>(event.mouseMove.x), 
                             static_cast<float>(event.mouseMove.y));
        updateButtons(mousePos);
    }
}

void Screen::onEnter() {
    isActive = true;
}

void Screen::onExit() {
    isActive = false;
    if (weather) {
        weather->stopUpdateThread();
    }
}

void Screen::setScreenChangeCallback(std::function<void(ScreenType)> callback) {
    onScreenChange = callback;
}

void Screen::setQuitCallback(std::function<void()> callback) {
    onQuit = callback;
}

void Screen::addButton(const std::string& text, sf::Vector2f position, const sf::Font& font) {
    auto button = std::make_unique<Button>(text, font, 24);
    button->setPosition(position.x, position.y);
    buttons.push_back(std::move(button));
}

void Screen::updateButtons(const sf::Vector2f& mousePos) {
    for (auto& button : buttons) {
        button->updateHover(mousePos);
    }
}

// MainMenuScreen implementation
MainMenuScreen::MainMenuScreen(sf::Vector2u windowSize, const sf::Font& font) 
    : Screen(ScreenType::MainMenu, windowSize), backgroundLoaded(false) {
    setupButtons(font);
    weather->setWeatherType(WeatherType::Snow);
    weather->startUpdateThread();
    
    // Load background image
    if (backgroundTexture.loadFromFile("assets/mainmenubg2.png")) {
        backgroundSprite.setTexture(backgroundTexture);
        
        // Scale the background to fit the window
        sf::Vector2u textureSize = backgroundTexture.getSize();
        float scaleX = static_cast<float>(windowSize.x) / static_cast<float>(textureSize.x);
        float scaleY = static_cast<float>(windowSize.y) / static_cast<float>(textureSize.y);
        backgroundSprite.setScale(scaleX, scaleY);
        
        backgroundLoaded = true;
        std::cout << "Background image loaded successfully!" << std::endl;
    } else {
        std::cout << "Failed to load background image: assets/mainmenubg2.png" << std::endl;
    }
}

void MainMenuScreen::setupButtons(const sf::Font& font) {
    // Start Game button
    auto startButton = std::make_unique<Button>("Start Game", font, 24);
    startButton->setPosition(50, windowSize.y / 2);
    startButton->setBackgroundColor(sf::Color(50, 50, 50));
    startButton->setTextColor(sf::Color::White);
    startButton->setCallback([this]() {
        std::cout << "Starting game..." << std::endl;
        if (onScreenChange) {
            screenEffects->startFadeIn(1.0f);
            onScreenChange(ScreenType::Game);
        }
    });
    buttons.push_back(std::move(startButton));

    // Settings button
    auto settingsButton = std::make_unique<Button>("Settings", font, 24);
    settingsButton->setPosition(50, windowSize.y / 2 + 60);
    settingsButton->setBackgroundColor(sf::Color(50, 50, 50));
    settingsButton->setTextColor(sf::Color::White);
    settingsButton->setCallback([this]() {
        std::cout << "Opening settings..." << std::endl;
        if (onScreenChange) {
            onScreenChange(ScreenType::Settings);
        }
    });
    buttons.push_back(std::move(settingsButton));

    // Quit button
    auto quitButton = std::make_unique<Button>("Quit", font, 24);
    quitButton->setPosition(50, windowSize.y / 2 + 120);
    quitButton->setBackgroundColor(sf::Color(50, 50, 50));
    quitButton->setTextColor(sf::Color::White);
    quitButton->setCallback([this]() {
        std::cout << "Quitting game..." << std::endl;
        if (onQuit) {
            onQuit();
        }
    });
    buttons.push_back(std::move(quitButton));
}

void MainMenuScreen::update(float deltaTime) {
    Screen::update(deltaTime);
    // Add any main menu specific updates here
}

void MainMenuScreen::render(sf::RenderWindow& window) {
    // Draw background image first
    if (backgroundLoaded) {
        window.draw(backgroundSprite);
    }
    
    // Then draw the base screen elements (weather, buttons, effects)
    Screen::render(window);
    // Add any main menu specific rendering here
}

void MainMenuScreen::onEnter() {
    Screen::onEnter();
    screenEffects->startFadeIn(2.0f);
    std::cout << "Entered Main Menu" << std::endl;
}

// GameScreen implementation
GameScreen::GameScreen(sf::Vector2u windowSize) 
    : Screen(ScreenType::Game, windowSize) {
    // No weather for game screen by default
    weather->setWeatherType(WeatherType::None);
    weather->startUpdateThread();
}

void GameScreen::update(float deltaTime) {
    Screen::update(deltaTime);
    // Add game-specific updates here
}

void GameScreen::render(sf::RenderWindow& window) {
    // Game screen background (you can customize this)
    sf::RectangleShape background;
    background.setSize(sf::Vector2f(static_cast<float>(windowSize.x), 
                                   static_cast<float>(windowSize.y)));
    background.setFillColor(sf::Color(20, 40, 60)); // Dark blue background
    window.draw(background);

    // Game-specific rendering
    Screen::render(window);
    
    // Add game objects rendering here
}

void GameScreen::handleEvent(const sf::Event& event) {
    Screen::handleEvent(event);
    
    // Handle game-specific events
    if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::Escape) {
            if (onScreenChange) {
                onScreenChange(ScreenType::MainMenu);
            }
        }
    }
}

void GameScreen::onEnter() {
    Screen::onEnter();
    std::cout << "Entered Game Screen" << std::endl;
}

// ScreenManager implementation
ScreenManager::ScreenManager(sf::Vector2u windowSize, const sf::Font& font) 
    : currentScreen(nullptr), windowSize(windowSize) {
    setupScreens(font);
    changeScreen(ScreenType::MainMenu);
}

void ScreenManager::setupScreens(const sf::Font& font) {
    // Create all screens
    screens.push_back(std::make_unique<MainMenuScreen>(windowSize, font));
    screens.push_back(std::make_unique<GameScreen>(windowSize));

    // Set up screen change callbacks
    for (auto& screen : screens) {
        screen->setScreenChangeCallback([this](ScreenType newScreen) {
            changeScreen(newScreen);
        });
        screen->setQuitCallback([this]() {
            if (onQuit) {
                onQuit();
            }
        });
    }
}

void ScreenManager::update(float deltaTime) {
    if (currentScreen) {
        currentScreen->update(deltaTime);
    }
}

void ScreenManager::render(sf::RenderWindow& window) {
    if (currentScreen) {
        currentScreen->render(window);
    }
}

void ScreenManager::handleEvent(const sf::Event& event) {
    if (currentScreen) {
        currentScreen->handleEvent(event);
    }
}

void ScreenManager::changeScreen(ScreenType newScreen) {
    // Exit current screen
    if (currentScreen) {
        currentScreen->onExit();
    }

    // Find and activate new screen
    for (auto& screen : screens) {
        if (screen->getType() == newScreen) {
            currentScreen = screen.get();
            currentScreen->onEnter();
            break;
        }
    }
}

void ScreenManager::setQuitCallback(std::function<void()> callback) {
    onQuit = callback;
}

ScreenType ScreenManager::getCurrentScreenType() const {
    return currentScreen ? currentScreen->getType() : ScreenType::MainMenu;
}
