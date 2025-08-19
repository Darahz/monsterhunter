#include "../include/Window.h"
#include "../include/Inventory.h"
#include "../include/Player.h"
#include "../include/Button.h"
#include "../include/Weather.h"
#include "../include/Screeneffects.h"

#include <unistd.h>

int main(){
    Window window;
    Player player;
    Weather weather;
    ScreenEffects screenEffects;
    weather.setWeatherType(WeatherType::Snow);
    

    if (!window.initialize(WindowSize::HD_720)) {
        return -1;
    }
    weather.setWindowSize(window.getSize().x, window.getSize().y);
    Button startButton("Start Game", window.font, 24);
    startButton.setPosition(200, 200);
    startButton.setCallback([&]() {
    });

    screenEffects.startFadeIn(10.0f);
    while (window.isOpen()) {
        sf::Event event;
        while (window.window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.window.close();
            }
            startButton.handleEvent(event);                      
        }
        
        player.update();
        screenEffects.update(0.1f / 240.0f);
        
        window.clear();
        weather.update(0.1f / 240.0f);
        weather.draw(window.window);
        startButton.render(window.window);
        player.render();
        screenEffects.fadeIn(window.window);  // Draw fade effect on top
        
        window.render();
    }

    return 0;
}