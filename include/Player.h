// Player.h
#pragma once
#include <string>
#include <thread>
#include <chrono>
#include <termios.h>
#include <unistd.h>
#include "Inventory.h"

class World; // forward declare

class Player {
private:
    std::string sName;
    Inventory inventory;
    int iHealth = 100;

public:
    Player();
    void addItem(std::string name, int num) { inventory.addItem(name, num); }
    void removeItem(std::string name, int amt = 1) { inventory.removeItem(name, amt); }
    void listInventory() { inventory.listItems(); }

    int  getItemCount(std::string name) { return inventory.getItemCount(name); }
    bool hasItem(std::string name) { return inventory.hasItem(name); }

    std::vector<std::pair<std::string,int>> getInventoryList() const { return inventory.getItemsList(); }

    bool equipItem(std::string name) { return inventory.equipItem(name); }
    std::string getEquipped() const { return inventory.getEquipped(); }

    bool craftItem(std::string itemName);
    bool isDead();
    void TakeHealth(int iRemoval);
    void AddHealth(int iAdd);
    void rest(World& world);
    void printHealth();
    int getHealth() const { return iHealth; }
};
