#pragma once

#include "Creature.h"
#include "Inventory.h"
#include "Injury.h"
#include <SFML/System.hpp>

class Player : public Creature {
public:
    float timeAlive;
    Player();
    ~Player();

    Inventory& getInventory() { return inventory; }
    /// @brief Updates the player's state, such as time alive.
    void update();
    void render();
    // list of current injuries
    std::vector<Injury> injuries;
private:
    Inventory inventory;
    sf::Clock clock;
};
