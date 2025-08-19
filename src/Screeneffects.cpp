#include "../include/Screeneffects.h"
#include <SFML/Graphics.hpp>
#include <algorithm>

ScreenEffects::ScreenEffects() : fadeDuration(0.0f), fadeElapsedTime(0.0f), fadeInActive(false) {
    // Load shaders
    vignetteShader.loadFromFile("shaders/vignette.frag", sf::Shader::Fragment);
    blurShader.loadFromFile("shaders/blur.frag", sf::Shader::Fragment);
    colorGradingShader.loadFromFile("shaders/color_grading.frag", sf::Shader::Fragment);
    
    // Initialize fade overlay
    fadeOverlay.setFillColor(sf::Color::Black);
}

ScreenEffects::~ScreenEffects() {
}

void ScreenEffects::applyVignette(sf::RenderWindow& window) {
    // Apply vignette effect
    window.draw(sf::Sprite(), &vignetteShader);
}

void ScreenEffects::applyBlur(sf::RenderWindow& window) {
    // Apply blur effect
    window.draw(sf::Sprite(), &blurShader);
}

void ScreenEffects::applyColorGrading(sf::RenderWindow& window) {
    // Apply color grading effect
    window.draw(sf::Sprite(), &colorGradingShader);
}

void ScreenEffects::fadeIn(sf::RenderWindow& window) {
    if (fadeInActive) {
        // Set the overlay size to match the window
        sf::Vector2u windowSize = window.getSize();
        fadeOverlay.setSize(sf::Vector2f(static_cast<float>(windowSize.x), static_cast<float>(windowSize.y)));
        
        // Calculate fade progress (0.0 = fully black, 1.0 = fully transparent)
        float fadeProgress = fadeElapsedTime / fadeDuration;
        fadeProgress = std::min(fadeProgress, 1.0f);
        
        // Calculate alpha value (255 = fully opaque/black, 0 = fully transparent)
        sf::Uint8 alpha = static_cast<sf::Uint8>(255 * (1.0f - fadeProgress));
        
        // Set the overlay color with calculated alpha
        sf::Color overlayColor = sf::Color::Black;
        overlayColor.a = alpha;
        fadeOverlay.setFillColor(overlayColor);
        
        // Draw the overlay
        window.draw(fadeOverlay);
    }
}

void ScreenEffects::startFadeIn(float duration) {
    fadeDuration = duration;
    fadeElapsedTime = 0.0f;
    fadeInActive = true;
}

void ScreenEffects::update(float deltaTime) {
    if (fadeInActive) {
        fadeElapsedTime += deltaTime;
        
        // Stop fade-in when duration is reached
        if (fadeElapsedTime >= fadeDuration) {
            fadeInActive = false;
        }
    }
}

bool ScreenEffects::isFadingIn() const {
    return fadeInActive;
}
