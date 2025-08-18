// Creature.h
#pragma once
#include <string>
#include <thread>
#include <chrono>
#include <termios.h>
#include <unistd.h>
#include <iostream>
#include "Inventory.h"

class World;

class Creature {
protected:
    std::string sName;
    Inventory inventory;
    int iHealth = 100;

public:
    Creature();
    virtual ~Creature() = default;
    void listInventory();
    
    std::vector<std::pair<std::string,int>> getInventoryList() const;
    
    virtual bool isDead();
    virtual void TakeHealth(int iRemoval);
    virtual void AddHealth(int iAdd);
    virtual void printHealth();
    int getHealth() const;
};
