#include "../include/Window.h"
#include "../include/Inventory.h"
#include "../include/Player.h"
#include "../include/Button.h"
#include "../include/Weather.h"

#include <unistd.h>

int main(){
    Window window;
    Player player;
    Weather weather;
    weather.setWeatherType(WeatherType::Snow);
    

    if (!window.initialize(WindowSize::HD_720)) {
        return -1;
    }
    weather.setWindowSize(window.getSize().x, window.getSize().y);
    Button startButton("Start Game", window.font, 24);
    startButton.setPosition(200, 200);
    startButton.setCallback([&]() {
        std::cout << "Start Game button clicked!" << std::endl;
    });

    while (window.isOpen()) {
        sf::Event event;
        while (window.window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.window.close();
            }
            startButton.handleEvent(event);                      
        }
        
        player.update();
        
        window.clear();
        weather.update(0.1f / 240.0f);
        weather.draw(window.window);
        startButton.render(window.window);
        player.render();
        
        window.render();
    }

    return 0;
}