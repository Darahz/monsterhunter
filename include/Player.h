#pragma once

#include "Creature.h"
#include "Inventory.h"
#include <SFML/System.hpp>

class Player : public Creature {
public:
    float timeAlive;
    Player();
    ~Player();

    Inventory& getInventory() { return inventory; }

    void update();
    void render();

private:
    Inventory inventory;
    sf::Clock clock;
};
