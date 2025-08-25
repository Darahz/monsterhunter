#include "../include/Creature.h"
#include "../include/Utils.h"
#include <iostream>

Creature::Creature() {}

void Creature::listInventory() { 
    inventory.listItems(); 
}

std::vector<std::pair<Item,int>> Creature::getInventoryList() const { 
    return inventory.getItemsList(); 
}

bool Creature::isDead() { 
    return iHealth <= 0; 
}

void Creature::TakeHealth(int iRemoval) { 
    iHealth -= iRemoval; 
}

void Creature::AddHealth(int iAdd) { 
    iHealth += iAdd; 
}

void Creature::printHealth() { 
    std::cout << "Health: " << iHealth << std::endl; 
}

int Creature::getHealth() const { 
    return iHealth; 
}