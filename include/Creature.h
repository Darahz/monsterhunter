#pragma once
#include <iostream>

class Creature {
public:
    int getHealth() const { return health; }
    std::string getName() const { return name; }

    void setHealth(int newHealth) { health = newHealth; }
    void setName(const std::string& newName) { name = newName; }
    void hurt(int damage) { health -= damage; }
    void heal(int amount) { health += amount; }

private:
    int health = 100;
    std::string name = "Creature";
};
