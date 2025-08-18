#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>

// Class to include all creatures including player and enemies
class Creature {
public:
    int getHealth() const { return health; }
    std::string getName() const { return name; }

    void setHealth(int newHealth) { health = newHealth; }
    void hurt(int damage) { health -= damage; }
    void heal(int amount) { health += amount; }

private:
    int health = 100;
    std::string name = "Creature";
};
