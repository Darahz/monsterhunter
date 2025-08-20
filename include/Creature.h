#pragma once
#include <iostream>

class Creature {
public:
    /// @brief Gets the health of the creature.
    /// @return The health of the creature.
    int getHealth() const { return health; }
    /// @brief Gets the name of the creature.
    /// @return The name of the creature.
    std::string getName() const { return name; }
    /// @brief Sets the creature's health.
    /// @param newHealth The new health value for the creature.
    void setHealth(int newHealth) { health = newHealth; }
    /// @brief Sets the creature's name.
    /// @param newName The new name for the creature.
    void setName(const std::string& newName) { name = newName; }
    /// @brief Reduces the creature's health by the specified damage amount.
    /// @param damage The amount of damage to inflict.
    void hurt(int damage) { health -= damage; }
    /// @brief Increases the creature's health by the specified amount.
    /// @param amount The amount to heal.
    void heal(int amount) { health += amount; }

private:
    int health = 100;
    std::string name = "Creature";
};
