#include "../include/Window.h"
#include "../include/Inventory.h"
#include "../include/Player.h"
#include "../include/Screen.h"

#include <unistd.h>

int main(){
    Window window;
    Player player;

    if (!window.initialize(WindowSize::HD_720)) {
        return -1;
    }

    // Create screen manager
    ScreenManager screenManager(window.getSize(), window.font);

    // Set up quit callback
    screenManager.setQuitCallback([&window]() {
        window.window.close();
    });

    while (window.isOpen()) {
        sf::Event event;
        while (window.window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.window.close();
            }
            
            // Handle screen events
            screenManager.handleEvent(event);
        }
        
        // Update systems
        float deltaTime = 0.1f / 240.0f;
        player.update();
        screenManager.update(deltaTime);
        
        // Render
        window.clear();
        
        // Render current screen
        screenManager.render(window.window);
        
        // Render player only on game screen
        if (screenManager.getCurrentScreenType() == ScreenType::Game) {
            player.render();
        }
        
        window.render();
    }

    return 0;
}