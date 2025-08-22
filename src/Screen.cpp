#include "../include/Screen.h"
#include "../include/Window.h"
#include "../include/NewGame.h"
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
        screenEffects->fadeOut(window);
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
    auto button = std::make_unique<UI::Button>(text, font, 24);
    button->setPosition(position.x, position.y);
    buttons.push_back(std::move(button));
}

void Screen::updateButtons(const sf::Vector2f& mousePos) {
    for (auto& button : buttons) {
        button->updateHover(mousePos);
    }
}

void Screen::updateWindowSize(sf::Vector2u newSize) {
    windowSize = newSize;
    if (weather) {
        weather->setWindowSize(static_cast<float>(newSize.x), static_cast<float>(newSize.y));
    }
}

// MainMenuScreen implementation
MainMenuScreen::MainMenuScreen(sf::Vector2u windowSize, const sf::Font& font) 
    : Screen(ScreenType::MainMenu, windowSize), backgroundLoaded(false), storedFont(&font) {
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
    }
}

void MainMenuScreen::setupButtons(const sf::Font& font) {
    // Start Game button
    auto startButton = std::make_unique<UI::Button>("New Game", font, 24);
    startButton->setPosition(50, windowSize.y / 2);
    startButton->setBackgroundColor(sf::Color(50, 50, 50));
    startButton->setTextColor(sf::Color::White);
    startButton->setCallback([this]() {
        if (onScreenChange) {
            screenEffects->startFadeOut(1.0f, [this]() {
                onScreenChange(ScreenType::NewGame);
            });
        }
    });
    buttons.push_back(std::move(startButton));

    // Settings button
    auto settingsButton = std::make_unique<UI::Button>("Settings", font, 24);
    settingsButton->setPosition(50, windowSize.y / 2 + 60);
    settingsButton->setBackgroundColor(sf::Color(50, 50, 50));
    settingsButton->setTextColor(sf::Color::White);
    settingsButton->setCallback([this]() {
        if (onScreenChange) {
            screenEffects->startFadeOut(1.5f, [this]() {
                onScreenChange(ScreenType::Settings);
            });
        }
    });
    buttons.push_back(std::move(settingsButton));

    // Quit button
    auto quitButton = std::make_unique<UI::Button>("Quit", font, 24);
    quitButton->setPosition(50, windowSize.y / 2 + 120);
    quitButton->setBackgroundColor(sf::Color(50, 50, 50));
    quitButton->setTextColor(sf::Color::White);
    quitButton->setCallback([this]() {
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
}

void MainMenuScreen::updateWindowSize(sf::Vector2u newSize) {
    Screen::updateWindowSize(newSize);
    updateBackgroundScale();
    
    // Clear and recreate buttons with new positions
    buttons.clear();
    if (storedFont) {
        setupButtons(*storedFont);
    }
}

void MainMenuScreen::updateBackgroundScale() {
    if (backgroundLoaded) {
        sf::Vector2u textureSize = backgroundTexture.getSize();
        float scaleX = static_cast<float>(windowSize.x) / static_cast<float>(textureSize.x);
        float scaleY = static_cast<float>(windowSize.y) / static_cast<float>(textureSize.y);
        backgroundSprite.setScale(scaleX, scaleY);
    }
}

// GameScreen implementation
GameScreen::GameScreen(sf::Vector2u windowSize) 
    : Screen(ScreenType::Game, windowSize) {
    weather->setWeatherType(WeatherType::None);
    weather->startUpdateThread();
}

void GameScreen::update(float deltaTime) {
    Screen::update(deltaTime);
}

void GameScreen::render(sf::RenderWindow& window) {
    sf::RectangleShape background;
    background.setSize(sf::Vector2f(static_cast<float>(windowSize.x), 
                                   static_cast<float>(windowSize.y)));
    background.setFillColor(sf::Color(20, 40, 60));
    window.draw(background);

    Screen::render(window);
}

void GameScreen::handleEvent(const sf::Event& event) {
    Screen::handleEvent(event);
    
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

void GameScreen::updateWindowSize(sf::Vector2u newSize) {
    Screen::updateWindowSize(newSize);
}

// SettingsScreen implementation
SettingsScreen::SettingsScreen(sf::Vector2u windowSize, const sf::Font& font, Window* windowRef)
    : Screen(ScreenType::Settings, windowSize), windowReference(windowRef), storedFont(&font), isChangingSettings(false) {
    if (windowReference) {
        setupUI(font);
    }
    weather->setWeatherType(WeatherType::None);
    weather->startUpdateThread();
}

void SettingsScreen::setupUI(const sf::Font& font) {
    float startY = 100.0f;
    float spacing = 80.0f;
    float rightColumnX = 450.0f;
    
    // Volume slider
    auto volumeLabel = std::make_unique<UI::Label>("Master Volume", font, 18);
    volumeLabel->setPosition(50, startY);
    labels.push_back(std::move(volumeLabel));
    
    auto volumeSlider = std::make_unique<UI::Slider>(0.0f, 100.0f, 75.0f, font);
    volumeSlider->setPosition(50, startY + 25);
    volumeSlider->setSize(300, 10);
    volumeSlider->setCallback([](float value) {
        std::cout << "Volume set to: " << value << "%" << std::endl;
    });
    sliders.push_back(std::move(volumeSlider));
    
    // FPS Limit slider
    auto fpsLabel = std::make_unique<UI::Label>("FPS Limit", font, 18);
    fpsLabel->setPosition(50, startY + spacing);
    labels.push_back(std::move(fpsLabel));
    
    // Calculate maximum FPS based on system capabilities
    float maxFPS = 300.0f; // Default fallback
    float currentFPS = 60.0f; // Default current FPS
    if (windowReference) {
        maxFPS = static_cast<float>(windowReference->getMaxPracticalFPS());
        currentFPS = static_cast<float>(windowReference->getDesiredFramerateLimit());
    }
    
    auto fpsSlider = std::make_unique<UI::Slider>(30.0f, maxFPS, currentFPS, font);
    fpsSlider->setPosition(50, startY + spacing + 25);
    fpsSlider->setSize(300, 10);
    fpsSlider->setCallback([this](float value) {
        if (windowReference) {
            windowReference->setFramerateLimit(static_cast<int>(value));
        } else {
        }
    });
    sliders.push_back(std::move(fpsSlider));
    
    // Music Volume slider
    auto musicLabel = std::make_unique<UI::Label>("Music Volume", font, 18);
    musicLabel->setPosition(50, startY + spacing * 2);
    labels.push_back(std::move(musicLabel));
    
    auto musicSlider = std::make_unique<UI::Slider>(0.0f, 100.0f, 50.0f, font);
    musicSlider->setPosition(50, startY + spacing * 2 + 25);
    musicSlider->setSize(300, 10);
    musicSlider->setCallback([](float value) {
        std::cout << "Music Volume set to: " << value << "%" << std::endl;
    });
    sliders.push_back(std::move(musicSlider));
    
    // Screen Resolution dropdown
    auto resolutionLabel = std::make_unique<UI::Label>("Screen Resolution", font, 18);
    resolutionLabel->setPosition(rightColumnX, startY);
    labels.push_back(std::move(resolutionLabel));
    
    auto resolutionDropdown = std::make_unique<UI::Dropdown>(font, 200);
    resolutionDropdown->setPosition(rightColumnX, startY + 25);
    
    // Use Window utility functions to populate dropdown
    if (windowReference) {
        auto resolutionNames = windowReference->getAvailableResolutionNames();
        for (const auto& name : resolutionNames) {
            resolutionDropdown->addItem(name);
        }
    } else {
        // Fallback if no window reference (shouldn't happen in normal usage)
        resolutionDropdown->addItem("800x600 (SVGA)");
    }
    
    // Set the dropdown to match the current window resolution
    int currentResolutionIndex = 1; // Default to SVGA
    if (windowReference) {
        WindowSize currentSize = windowReference->getCurrentWindowSize();
        currentResolutionIndex = windowReference->getIndexFromWindowSize(currentSize);
        std::cout << "Setting dropdown to index: " << currentResolutionIndex << " for WindowSize: " << static_cast<int>(currentSize) << std::endl;
    }
    resolutionDropdown->setSelectedIndex(currentResolutionIndex);
    
    resolutionDropdown->setCallback([this](int index, const std::string& item) {
        if (windowReference) {
            isChangingSettings = true; // Prevent UI recreation during this change
            WindowSize newSize = windowReference->getWindowSizeFromIndex(index);
            windowReference->changeResolution(newSize);
            isChangingSettings = false; // Re-enable UI recreation
        } else {
            std::cout << "Resolution changed to: " << item << " (index: " << index << ")" << std::endl;
        }
    });
    dropdowns.push_back(std::move(resolutionDropdown));
    
    // Fullscreen checkbox
    bool currentFullscreenState = false;
    if (windowReference) {
        currentFullscreenState = windowReference->isFullscreen();
    }
    auto fullscreenCheckbox = std::make_unique<UI::Checkbox>("Enable Fullscreen", font, currentFullscreenState);
    fullscreenCheckbox->setPosition(rightColumnX, startY + spacing);
    fullscreenCheckbox->setCallback([this](bool checked) {
        if (windowReference) {
            isChangingSettings = true; // Prevent UI recreation during this change
            windowReference->setFullscreen(checked);
            isChangingSettings = false; // Re-enable UI recreation
        } else {
            std::cout << "Fullscreen " << (checked ? "enabled" : "disabled") << std::endl;
        }
    });
    checkboxes.push_back(std::move(fullscreenCheckbox));
    
    // VSync checkbox
    bool currentVSyncState = true; // Default
    if (windowReference) {
        currentVSyncState = windowReference->isVSyncEnabled();
    }
    auto vsyncCheckbox = std::make_unique<UI::Checkbox>("Enable VSync", font, currentVSyncState);
    vsyncCheckbox->setPosition(rightColumnX, startY + spacing + 40);
    vsyncCheckbox->setCallback([this](bool checked) {
        if (windowReference) {
            windowReference->setVSync(checked);
        } else {
            std::cout << "VSync " << (checked ? "enabled" : "disabled") << std::endl;
        }
    });
    checkboxes.push_back(std::move(vsyncCheckbox));
    
    // Show FPS checkbox
    bool currentFPSDisplayState = true; // Default
    if (windowReference) {
        currentFPSDisplayState = windowReference->isShowingFPS();
    }
    auto showFpsCheckbox = std::make_unique<UI::Checkbox>("Show FPS in Title", font, currentFPSDisplayState);
    showFpsCheckbox->setPosition(rightColumnX, startY + spacing + 80);
    showFpsCheckbox->setCallback([this](bool checked) {
        if (windowReference) {
            windowReference->setShowFPS(checked);
        } else {
            std::cout << "FPS display " << (checked ? "enabled" : "disabled") << std::endl;
        }
    });
    checkboxes.push_back(std::move(showFpsCheckbox));
    
    // Back button
    auto backButton = std::make_unique<UI::Button>("Back", font, 24);
    backButton->setPosition(50, startY + spacing * 3 + 50);
    backButton->setBackgroundColor(sf::Color(100, 100, 100));
    backButton->setTextColor(sf::Color::White);
    backButton->setCallback([this]() {
        std::cout << "Returning to main menu..." << std::endl;
        if (onScreenChange) {
            screenEffects->startFadeOut(1.0f, [this]() {
                onScreenChange(ScreenType::MainMenu);
            });
        }
    });
    buttons.push_back(std::move(backButton));
}

void SettingsScreen::update(float deltaTime) {
    Screen::update(deltaTime);
}

void SettingsScreen::render(sf::RenderWindow& window) {
    // Dark background
    sf::RectangleShape background;
    background.setSize(sf::Vector2f(static_cast<float>(windowSize.x), 
                                   static_cast<float>(windowSize.y)));
    background.setFillColor(sf::Color(30, 30, 30));
    window.draw(background);
    
    // Render labels
    for (auto& label : labels) {
        label->render(window);
    }
    
    // Render sliders
    for (auto& slider : sliders) {
        slider->render(window);
    }
    
    // Render checkboxes
    for (auto& checkbox : checkboxes) {
        checkbox->render(window);
    }
    
    // Render dropdowns
    for (auto& dropdown : dropdowns) {
        dropdown->render(window);
    }
    
    // Render base screen elements (buttons, effects)
    Screen::render(window);
}

void SettingsScreen::handleEvent(const sf::Event& event) {
    Screen::handleEvent(event);
    
    // Handle slider events
    for (auto& slider : sliders) {
        slider->handleEvent(event);
    }
    
    // Handle checkbox events
    for (auto& checkbox : checkboxes) {
        checkbox->handleEvent(event);
    }
    
    // Handle dropdown events
    for (auto& dropdown : dropdowns) {
        dropdown->handleEvent(event);
    }
    
    // Handle mouse movement for all components
    if (event.type == sf::Event::MouseMoved) {
        sf::Vector2f mousePos(static_cast<float>(event.mouseMove.x), 
                             static_cast<float>(event.mouseMove.y));
        for (auto& slider : sliders) {
            slider->updateHover(mousePos);
        }
        for (auto& checkbox : checkboxes) {
            checkbox->updateHover(mousePos);
        }
        for (auto& dropdown : dropdowns) {
            dropdown->updateHover(mousePos);
        }
    }
}

void SettingsScreen::onEnter() {
    Screen::onEnter();
    std::cout << "Entered Settings Screen" << std::endl;
}

void SettingsScreen::setWindowReference(Window* windowRef) {
    windowReference = windowRef;
    
    // Setup UI now that we have a proper window reference (if not already done)
    if (windowReference && storedFont && buttons.empty() && dropdowns.empty()) {
        setupUI(*storedFont);
    }
}

void SettingsScreen::updateWindowSize(sf::Vector2u newSize) {
    Screen::updateWindowSize(newSize);
    
    // Don't recreate UI if we're in the middle of changing settings internally
    if (isChangingSettings) {
        return;
    }
    
    // Clear all UI elements and recreate them with new positions
    buttons.clear();
    sliders.clear();
    labels.clear();
    checkboxes.clear();
    dropdowns.clear();
    
    if (storedFont) {
        setupUI(*storedFont);
    }
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
    screens.push_back(std::make_unique<NewGameScreen>(windowSize, font));
    screens.push_back(std::make_unique<GameScreen>(windowSize));
    screens.push_back(std::make_unique<SettingsScreen>(windowSize, font));

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

void ScreenManager::setWindowReference(Window* windowRef) {
    // Find the settings screen and set its window reference
    for (auto& screen : screens) {
        if (screen->getType() == ScreenType::Settings) {
            static_cast<SettingsScreen*>(screen.get())->setWindowReference(windowRef);
            break;
        }
    }
}

void ScreenManager::updateWindowSize(sf::Vector2u newSize) {
    windowSize = newSize;
    
    // Update all screens with new window size
    for (auto& screen : screens) {
        screen->updateWindowSize(newSize);
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
