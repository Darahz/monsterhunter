#pragma once

#include "Creature.h"
#include "Inventory.h"

class Player : public Creature {
public:
    Player();
    ~Player();

    Inventory& getInventory() { return inventory; }

private:
    Inventory inventory;

    void update();
    void render();
};
