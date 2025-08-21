#include "../include/Window.h"
#include "../include/Inventory.h"
#include "../include/Player.h"
#include "../include/Screen.h"
#include <chrono>
#include <algorithm>

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
    
    // Set window reference for settings screen
    screenManager.setWindowReference(&window);

    // Delta time calculation variables
    auto lastTime = std::chrono::steady_clock::now();

    while (window.isOpen()) {
        // Calculate delta time
        auto currentTime = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(currentTime - lastTime);
        float deltaTime = elapsed.count() / 1000000.0f; // Convert microseconds to seconds
        lastTime = currentTime;
        
        // Cap deltaTime to prevent large jumps (e.g., when window is minimized)
        deltaTime = std::min(deltaTime, 0.1f);
        
        sf::Event event;
        while (window.window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.window.close();
            }
            
            // Handle screen events
            screenManager.handleEvent(event);
        }
        
        // Update systems
        window.update();
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