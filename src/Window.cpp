#include "../include/Window.h"

Window::Window() : frameCount(0), currentFPS(0.0f) {
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
    sf::VideoMode videoMode = getVideoModeFromSize(size);
    baseTitle = "Monster Hunter v." + this->getVersion();
    window.setFramerateLimit(60);
    window.setVerticalSyncEnabled(true);
    window.create(videoMode, baseTitle);
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
        
        std::string title = baseTitle + " - FPS: " + std::to_string(static_cast<int>(currentFPS));
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
