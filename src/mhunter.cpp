#include "../include/Window.h"

int main(){
    Window window;
    if (!window.initialize()) {
        return -1;
    }

    while (window.isOpen()) {
        window.update();
        window.render();
    }

    return 0;
}