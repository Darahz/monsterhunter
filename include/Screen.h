#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include <functional>
#include "UIElements.h"
#include "Weather.h"
#include "Screeneffects.h"

// Forward declarations
class Window;
class NewGameScreen;

enum class ScreenType {
    MainMenu,
    NewGame,
    Game,
    Settings,
    Pause
};

class Screen {
public:
    Screen(ScreenType type, sf::Vector2u windowSize);
    virtual ~Screen() = default;

    // Core functionality
    virtual void update(float deltaTime);
    virtual void render(sf::RenderWindow& window);
    virtual void handleEvent(const sf::Event& event);
    virtual void onEnter();
    virtual void onExit();

    // Screen management
    ScreenType getType() const { return screenType; }
    void setActive(bool active) { isActive = active; }
    bool getActive() const { return isActive; }

    // Screen transition callback
    void setScreenChangeCallback(std::function<void(ScreenType)> callback);
    void setQuitCallback(std::function<void()> callback);
    
    // Window size management
    virtual void updateWindowSize(sf::Vector2u newSize);
    sf::Vector2u getWindowSize() const { return windowSize; }

protected:
    ScreenType screenType;
    sf::Vector2u windowSize;
    bool isActive;
    std::vector<std::unique_ptr<UI::Button>> buttons;
    std::unique_ptr<Weather> weather;
    std::unique_ptr<ScreenEffects> screenEffects;
    std::function<void(ScreenType)> onScreenChange;
    std::function<void()> onQuit;

    // Helper methods for derived classes
    void addButton(const std::string& text, sf::Vector2f position, const sf::Font& font);
    void updateButtons(const sf::Vector2f& mousePos);
};

// Specific screen implementations
class MainMenuScreen : public Screen {
public:
    MainMenuScreen(sf::Vector2u windowSize, const sf::Font& font);
    void update(float deltaTime) override;
    void render(sf::RenderWindow& window) override;
    void onEnter() override;
    void updateWindowSize(sf::Vector2u newSize) override;

private:
    void setupButtons(const sf::Font& font);
    void updateBackgroundScale();
    sf::Texture backgroundTexture;
    sf::Sprite backgroundSprite;
    bool backgroundLoaded;
    const sf::Font* storedFont;
};

class GameScreen : public Screen {
public:
    GameScreen(sf::Vector2u windowSize);
    void update(float deltaTime) override;
    void render(sf::RenderWindow& window) override;
    void handleEvent(const sf::Event& event) override;
    void onEnter() override;
    void updateWindowSize(sf::Vector2u newSize) override;
};

class SettingsScreen : public Screen {
public:
    SettingsScreen(sf::Vector2u windowSize, const sf::Font& font, Window* windowRef = nullptr);
    void update(float deltaTime) override;
    void render(sf::RenderWindow& window) override;
    void handleEvent(const sf::Event& event) override;
    void onEnter() override;
    void updateWindowSize(sf::Vector2u newSize) override;
    
    void setWindowReference(Window* windowRef);

private:
    void setupUI(const sf::Font& font);
    std::vector<std::unique_ptr<UI::Slider>> sliders;
    std::vector<std::unique_ptr<UI::Label>> labels;
    std::vector<std::unique_ptr<UI::Checkbox>> checkboxes;
    std::vector<std::unique_ptr<UI::Dropdown>> dropdowns;
    Window* windowReference;
    const sf::Font* storedFont;
    bool isChangingSettings; // Flag to prevent UI recreation during internal changes
};

// Screen Manager class
class ScreenManager {
public:
    ScreenManager(sf::Vector2u windowSize, const sf::Font& font);
    ~ScreenManager() = default;

    void update(float deltaTime);
    void render(sf::RenderWindow& window);
    void handleEvent(const sf::Event& event);
    void changeScreen(ScreenType newScreen);
    void setQuitCallback(std::function<void()> callback);
    void setWindowReference(Window* windowRef);
    void updateWindowSize(sf::Vector2u newSize);
    
    ScreenType getCurrentScreenType() const;

private:
    std::vector<std::unique_ptr<Screen>> screens;
    Screen* currentScreen;
    sf::Vector2u windowSize;
    std::function<void()> onQuit;

    void setupScreens(const sf::Font& font);
};