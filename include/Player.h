// Player.h
#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include <thread>
#include <chrono>
#include <termios.h>
#include <unistd.h>

class World; // forward declare

class Player {
private:
    std::string sName;
    std::unordered_map<std::string,int> inventory;
    int iHealth = 100;
    std::string equipped; // empty = nothing equipped

public:
    Player();
    void addItem(std::string name, int num);
    void removeItem(std::string name, int amt = 1);
    void listInventory();

    int  getItemCount(std::string name);
    bool hasItem(std::string name);

    // NEW: expose a snapshot for menus (name,count)
    std::vector<std::pair<std::string,int>> getInventoryList() const;

    // NEW: equip helpers
    bool equipItem(std::string name);
    std::string getEquipped() const { return equipped; }

    bool craftItem(std::string itemName);
    bool isDead();
    void TakeHealth(int iRemoval);
    void AddHealth(int iAdd);
    void rest(World& world);
};
