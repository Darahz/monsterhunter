#pragma once
#include <SFML/Graphics.hpp>
#include "UIElements.h"
#include <iostream>
#include <chrono>

enum class WindowSize {
    // Common 4:3 aspect ratio sizes
    VGA = 0,        // 640x480
    SVGA,           // 800x600
    XGA,            // 1024x768
    
    // Common 16:9 aspect ratio sizes
    HD_720,         // 1280x720
    HD_900,         // 1600x900
    HD_1080,        // 1920x1080
    
    // Common 16:10 aspect ratio sizes
    WXGA,           // 1280x800
    WSXGA,          // 1680x1050
    WUXGA,          // 1920x1200
    
    // Other common sizes
    SQUARE_800,     // 800x800
    SQUARE_1024,    // 1024x1024
    ULTRAWIDE       // 2560x1080
};

class Window {
public:

    sf::RenderWindow window;
    sf::Font font;

    Window();
    ~Window();

    bool initialize(WindowSize size = WindowSize::SVGA);
    void update();
    void updateFPS();
    void clear();
    void render();
    void loadFont(const std::string& fontPath);
    bool isOpen() const;
    std::string getVersion() const;
    sf::VideoMode getVideoModeFromSize(WindowSize size) const;
    sf::Vector2u getSize() const;
    
    // New functionality for settings
    void changeResolution(WindowSize newSize);
    void setFullscreen(bool fullscreen);
    void setVSync(bool enabled);
    void setShowFPS(bool show);
    void setFramerateLimit(int fps);
    
    // Callback for window size changes
    void setWindowSizeChangeCallback(std::function<void(sf::Vector2u)> callback);
    
    // Getters for current settings
    bool isFullscreen() const;
    bool isVSyncEnabled() const;
    bool isShowingFPS() const;
    WindowSize getCurrentWindowSize() const;
    int getDesiredFramerateLimit() const;
    
    // Utility functions for UI dropdown integration
    std::vector<std::string> getAvailableResolutionNames() const;
    std::string getResolutionName(WindowSize size) const;
    WindowSize getWindowSizeFromIndex(int index) const;
    int getIndexFromWindowSize(WindowSize size) const;
    
    // FPS utilities
    int getMaxPracticalFPS() const;

private:
    // Just use todays date cuz im lazy
    int majorVersion = 8;
    int minorVersion = 18;
    int patchVersion = 0;
    
    // FPS tracking
    std::chrono::steady_clock::time_point lastTime;
    int frameCount;
    float currentFPS;
    std::string baseTitle;
    
    // Window settings
    WindowSize currentSize;
    bool fullscreenMode;
    bool vsyncEnabled;
    bool showFPS;
    int desiredFramerateLimit; // Store the desired FPS limit for when VSync is disabled
    
    // Callback for window size changes
    std::function<void(sf::Vector2u)> onWindowSizeChange;
};
