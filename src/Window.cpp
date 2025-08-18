#include "../include/Window.h"

Window::Window() {
    // load a standard font from linux
    loadFont("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf");
}

Window::~Window() {
    
}

std::string Window::getVersion() const {
    return std::to_string(majorVersion) + "." + std::to_string(minorVersion) + "." + std::to_string(patchVersion);
}

bool Window::initialize() {

    window.create(sf::VideoMode(800, 600), "Monster Hunter v." + this->getVersion());
    return window.isOpen();
}

void Window::update() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window.close();
        }
    }
}

void Window::render() {
    window.clear();
    
    window.display();
}

void Window::loadFont(const std::string& fontPath) {
    if (!font.loadFromFile(fontPath)) {
        std::cout << "Failed to load font from " << fontPath << std::endl;
    }
}

bool Window::isOpen() const {
    return window.isOpen();
}
