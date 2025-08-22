#include "../include/Screeneffects.h"
#include <SFML/Graphics.hpp>
#include <algorithm>

ScreenEffects::ScreenEffects() : fadeDuration(0.0f), fadeElapsedTime(0.0f), fadeInActive(false),
                               fadeOutDuration(0.0f), fadeOutElapsedTime(0.0f), fadeOutActive(false) {
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

void ScreenEffects::fadeOut(sf::RenderWindow& window) {
    if (fadeOutActive) {
        // Set the overlay size to match the window
        sf::Vector2u windowSize = window.getSize();
        fadeOverlay.setSize(sf::Vector2f(static_cast<float>(windowSize.x), static_cast<float>(windowSize.y)));
        
        // Calculate fade progress (0.0 = fully transparent, 1.0 = fully black)
        float fadeProgress = fadeOutElapsedTime / fadeOutDuration;
        fadeProgress = std::min(fadeProgress, 1.0f);
        
        // Calculate alpha value (0 = fully transparent, 255 = fully opaque/black)
        sf::Uint8 alpha = static_cast<sf::Uint8>(255 * fadeProgress);
        
        // Set the overlay color with calculated alpha
        sf::Color overlayColor = sf::Color::Black;
        overlayColor.a = alpha;
        fadeOverlay.setFillColor(overlayColor);
        
        // Draw the overlay
        window.draw(fadeOverlay);
    }
}

void ScreenEffects::startFadeIn(float duration, std::function<void()> onComplete) {
    fadeDuration = duration;
    fadeElapsedTime = 0.0f;
    fadeInActive = true;
    fadeCompleteCallback = onComplete;
}

void ScreenEffects::startFadeOut(float duration, std::function<void()> onComplete) {
    fadeOutDuration = duration;
    fadeOutElapsedTime = 0.0f;
    fadeOutActive = true;
    fadeOutCompleteCallback = onComplete;
}

void ScreenEffects::update(float deltaTime) {
    // Handle fade-in
    if (fadeInActive) {
        fadeElapsedTime += deltaTime;
        
        // Stop fade-in when duration is reached
        if (fadeElapsedTime >= fadeDuration) {
            fadeInActive = false;
            
            // Call the completion callback if set
            if (fadeCompleteCallback) {
                fadeCompleteCallback();
                fadeCompleteCallback = nullptr; // Clear callback after use
            }
        }
    }
    
    // Handle fade-out
    if (fadeOutActive) {
        fadeOutElapsedTime += deltaTime;
        
        // Stop fade-out when duration is reached
        if (fadeOutElapsedTime >= fadeOutDuration) {
            fadeOutActive = false;
            
            // Call the completion callback if set
            if (fadeOutCompleteCallback) {
                fadeOutCompleteCallback();
                fadeOutCompleteCallback = nullptr; // Clear callback after use
            }
        }
    }
}

bool ScreenEffects::isFadingIn() const {
    return fadeInActive;
}

bool ScreenEffects::isFadingOut() const {
    return fadeOutActive;
}
