#pragma once

#include <SFML/Graphics.hpp>

class ScreenEffects {
public:
    ScreenEffects();
    ~ScreenEffects();

    void applyVignette(sf::RenderWindow& window);
    void applyBlur(sf::RenderWindow& window);
    void applyColorGrading(sf::RenderWindow& window);
    void fadeIn(sf::RenderWindow& window);
    void startFadeIn(float duration);
    void update(float deltaTime);
    bool isFadingIn() const;

private:
    sf::Shader vignetteShader;
    sf::Shader blurShader;
    sf::Shader colorGradingShader;
    
    // Fade-in effect variables
    sf::RectangleShape fadeOverlay;
    float fadeDuration;
    float fadeElapsedTime;
    bool fadeInActive;
};
