#pragma once

#include <SFML/Graphics.hpp>
#include <functional>

class ScreenEffects {
public:
    ScreenEffects();
    ~ScreenEffects();

    void applyVignette(sf::RenderWindow& window);
    void applyBlur(sf::RenderWindow& window);
    void applyColorGrading(sf::RenderWindow& window);
    void fadeIn(sf::RenderWindow& window);
    void fadeOut(sf::RenderWindow& window);
    void startFadeIn(float duration, std::function<void()> onComplete = nullptr);
    void startFadeOut(float duration, std::function<void()> onComplete = nullptr);
    void update(float deltaTime);
    bool isFadingIn() const;
    bool isFadingOut() const;

private:
    sf::Shader vignetteShader;
    sf::Shader blurShader;
    sf::Shader colorGradingShader;
    
    // Fade-in effect variables
    sf::RectangleShape fadeOverlay;
    float fadeDuration;
    float fadeElapsedTime;
    bool fadeInActive;
    
    // Fade-out effect variables
    float fadeOutDuration;
    float fadeOutElapsedTime;
    bool fadeOutActive;
    
    // Callbacks for fade completion
    std::function<void()> fadeCompleteCallback;
    std::function<void()> fadeOutCompleteCallback;
};
