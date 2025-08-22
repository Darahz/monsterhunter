#include "../include/Window.h"

Window::Window() : frameCount(0), currentFPS(0.0f), currentSize(WindowSize::SVGA), 
                 fullscreenMode(false), vsyncEnabled(true), showFPS(true) {
    loadFont("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf");
    lastTime = std::chrono::steady_clock::now();
}

Window::~Window() {
    
}

std::string Window::getVersion() const {
    return std::to_string(majorVersion) + "." + std::to_string(minorVersion) + "." + std::to_string(patchVersion);
}

sf::VideoMode Window::getVideoModeFromSize(WindowSize size) const {
    switch (size) {
        case WindowSize::VGA:        return sf::VideoMode(640, 480);
        case WindowSize::SVGA:       return sf::VideoMode(800, 600);
        case WindowSize::XGA:        return sf::VideoMode(1024, 768);
        case WindowSize::HD_720:     return sf::VideoMode(1280, 720);
        case WindowSize::HD_900:     return sf::VideoMode(1600, 900);
        case WindowSize::HD_1080:    return sf::VideoMode(1920, 1080);
        case WindowSize::WXGA:       return sf::VideoMode(1280, 800);
        case WindowSize::WSXGA:      return sf::VideoMode(1680, 1050);
        case WindowSize::WUXGA:      return sf::VideoMode(1920, 1200);
        case WindowSize::SQUARE_800: return sf::VideoMode(800, 800);
        case WindowSize::SQUARE_1024:return sf::VideoMode(1024, 1024);
        case WindowSize::ULTRAWIDE:  return sf::VideoMode(2560, 1080);
        default:                     return sf::VideoMode(800, 600); // Default to SVGA
    }
}

bool Window::initialize(WindowSize size) {
    currentSize = size;
    sf::VideoMode videoMode = getVideoModeFromSize(size);
    baseTitle = "Monster Hunter v." + this->getVersion();
    window.setFramerateLimit(60);
    window.setVerticalSyncEnabled(vsyncEnabled);
    
    sf::Uint32 style = fullscreenMode ? sf::Style::Fullscreen : sf::Style::Default;
    window.create(videoMode, baseTitle, style);
    return window.isOpen();
}

void Window::update() {
    updateFPS();
}

void Window::updateFPS() {
    frameCount++;
    auto currentTime = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastTime);
    
    if (elapsed.count() >= 1000) {
        currentFPS = frameCount / (elapsed.count() / 1000.0f);
        frameCount = 0;
        lastTime = currentTime;
        
        std::string title = baseTitle;
        if (showFPS) {
            title += " - FPS: " + std::to_string(static_cast<int>(currentFPS));
        }
        window.setTitle(title);
    }
}

void Window::render() {
    window.display();
}

void Window::clear() {
    window.clear();
}

void Window::loadFont(const std::string& fontPath) {
    if (!font.loadFromFile(fontPath)) {
        std::cerr << "Failed to load font from " << fontPath << std::endl;
        std::exit(1);
    }
}

bool Window::isOpen() const {
    return window.isOpen();
}

sf::Vector2u Window::getSize() const {
    return window.getSize();
}

// New functionality for settings
void Window::changeResolution(WindowSize newSize) {
    if (newSize == currentSize) return;
    
    currentSize = newSize;
    sf::VideoMode videoMode = getVideoModeFromSize(newSize);
    sf::Uint32 style = fullscreenMode ? sf::Style::Fullscreen : sf::Style::Default;
    
    // Store current window position if not fullscreen
    sf::Vector2i oldPosition = window.getPosition();
    
    // Get current title to preserve it
    std::string currentTitle = baseTitle;
    if (showFPS) {
        currentTitle += " - FPS: " + std::to_string(static_cast<int>(currentFPS));
    }
    
    window.create(videoMode, currentTitle, style);
    window.setVerticalSyncEnabled(vsyncEnabled);
    window.setFramerateLimit(60); // Keep current framerate limit
    
    // Restore position if not fullscreen
    if (!fullscreenMode) {
        window.setPosition(oldPosition);
    }
    
    std::cout << "Resolution changed to: " << videoMode.width << "x" << videoMode.height << std::endl;
    
    // Notify callback about window size change
    if (onWindowSizeChange) {
        onWindowSizeChange(sf::Vector2u(videoMode.width, videoMode.height));
    }
}

void Window::setFullscreen(bool fullscreen) {
    if (fullscreen == fullscreenMode) return;
    
    fullscreenMode = fullscreen;
    sf::VideoMode videoMode = getVideoModeFromSize(currentSize);
    sf::Uint32 style = fullscreenMode ? sf::Style::Fullscreen : sf::Style::Default;
    
    // Store current window position if switching from windowed
    sf::Vector2i oldPosition = window.getPosition();
    
    // Get current title to preserve it
    std::string currentTitle = baseTitle;
    if (showFPS) {
        currentTitle += " - FPS: " + std::to_string(static_cast<int>(currentFPS));
    }
    
    window.create(videoMode, currentTitle, style);
    window.setVerticalSyncEnabled(vsyncEnabled);
    window.setFramerateLimit(60);
    
    // Restore position if switching back to windowed
    if (!fullscreenMode) {
        window.setPosition(oldPosition);
    }
    
    std::cout << "Fullscreen " << (fullscreenMode ? "enabled" : "disabled") << std::endl;
    
    // Notify callback about window size change
    if (onWindowSizeChange) {
        onWindowSizeChange(sf::Vector2u(videoMode.width, videoMode.height));
    }
}

void Window::setVSync(bool enabled) {
    if (enabled == vsyncEnabled) return;
    
    vsyncEnabled = enabled;
    window.setVerticalSyncEnabled(vsyncEnabled);
    std::cout << "VSync " << (vsyncEnabled ? "enabled" : "disabled") << std::endl;
}

void Window::setShowFPS(bool show) {
    if (show == showFPS) return;
    
    showFPS = show;
    
    // Update title immediately
    std::string title = baseTitle;
    if (showFPS) {
        title += " - FPS: " + std::to_string(static_cast<int>(currentFPS));
    }
    window.setTitle(title);
    
    std::cout << "FPS display " << (showFPS ? "enabled" : "disabled") << std::endl;
}

void Window::setFramerateLimit(int fps) {
    window.setFramerateLimit(fps);
    std::cout << "Framerate limit set to: " << fps << std::endl;
}

// Getters for current settings
bool Window::isFullscreen() const {
    return fullscreenMode;
}

bool Window::isVSyncEnabled() const {
    return vsyncEnabled;
}

bool Window::isShowingFPS() const {
    return showFPS;
}

WindowSize Window::getCurrentWindowSize() const {
    return currentSize;
}

void Window::setWindowSizeChangeCallback(std::function<void(sf::Vector2u)> callback) {
    onWindowSizeChange = callback;
}

// Utility functions for UI dropdown integration
std::vector<std::string> Window::getAvailableResolutionNames() const {
    // Only include the resolutions that are commonly used in the dropdown
    return {
        getResolutionName(WindowSize::VGA),
        getResolutionName(WindowSize::SVGA),
        getResolutionName(WindowSize::XGA),
        getResolutionName(WindowSize::HD_720),
        getResolutionName(WindowSize::HD_900),
        getResolutionName(WindowSize::HD_1080),
        getResolutionName(WindowSize::WXGA),
        getResolutionName(WindowSize::WSXGA),
        getResolutionName(WindowSize::WUXGA)
    };
}

std::string Window::getResolutionName(WindowSize size) const {
    switch (size) {
        case WindowSize::VGA:        return "640x480 (VGA)";
        case WindowSize::SVGA:       return "800x600 (SVGA)";
        case WindowSize::XGA:        return "1024x768 (XGA)";
        case WindowSize::HD_720:     return "1280x720 (HD)";
        case WindowSize::HD_900:     return "1600x900 (HD+)";
        case WindowSize::HD_1080:    return "1920x1080 (Full HD)";
        case WindowSize::WXGA:       return "1280x800 (WXGA)";
        case WindowSize::WSXGA:      return "1680x1050 (WSXGA)";
        case WindowSize::WUXGA:      return "1920x1200 (WUXGA)";
        case WindowSize::SQUARE_800: return "800x800 (Square)";
        case WindowSize::SQUARE_1024:return "1024x1024 (Square)";
        case WindowSize::ULTRAWIDE:  return "2560x1080 (Ultrawide)";
        default:                     return "800x600 (SVGA)";
    }
}

WindowSize Window::getWindowSizeFromIndex(int index) const {
    // Map dropdown index to WindowSize enum for commonly used resolutions
    switch (index) {
        case 0: return WindowSize::VGA;
        case 1: return WindowSize::SVGA;
        case 2: return WindowSize::XGA;
        case 3: return WindowSize::HD_720;
        case 4: return WindowSize::HD_900;
        case 5: return WindowSize::HD_1080;
        case 6: return WindowSize::WXGA;
        case 7: return WindowSize::WSXGA;
        case 8: return WindowSize::WUXGA;
        default: return WindowSize::SVGA; // Fallback
    }
}

int Window::getIndexFromWindowSize(WindowSize size) const {
    // Map WindowSize enum to dropdown index for commonly used resolutions
    switch (size) {
        case WindowSize::VGA:      return 0;
        case WindowSize::SVGA:     return 1;
        case WindowSize::XGA:      return 2;
        case WindowSize::HD_720:   return 3;
        case WindowSize::HD_900:   return 4;
        case WindowSize::HD_1080:  return 5;
        case WindowSize::WXGA:     return 6;
        case WindowSize::WSXGA:    return 7;
        case WindowSize::WUXGA:    return 8;
        default:                   return 1; // Fallback to SVGA
    }
}
