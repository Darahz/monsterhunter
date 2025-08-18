#include "../include/Window.h"
#include "../include/Inventory.h"
#include "../include/Player.h"

#include <unistd.h>

int main(){
    Window window;
    Player player;

    sleep(2); // Sleep for 2 seconds to see the inventory output before the window opens

    if (!window.initialize()) {
        return -1;
    }

    while (window.isOpen()) {
        window.update();
        window.render();
    }

    return 0;
}