// Player.h
#pragma once
#include <string>
#include "Creature.h"
#include "ItemFactory.h"

class World; // forward declare


class Player : public Creature {
    std::string name;
public:
    Player();
    void setName(const std::string& n) { name = n; }
    std::string getName() const { return name; }


    void addItem(const Item& item, int num) { inventory.addItem(item, num); }
    void addItem(const std::string& name, int num) { inventory.addItem(getItemByName(name), num); }
    void removeItem(const std::string& name, int amt = 1) { inventory.removeItem(name, amt); }
    void listInventory() { inventory.listItems(); }

    int  getItemCount(const std::string& name) { return inventory.getItemCount(name); }
    bool hasItem(const std::string& name) { return inventory.hasItem(name); }
    void clearInventory();

    std::vector<std::pair<Item, int>> getInventoryList() const { return inventory.getItemsList(); }

    bool equipItem(const std::string& name) { return inventory.equipItem(name); }
    std::string getEquipped() const { return inventory.getEquipped(); }

    bool craftItem(std::string itemName);
    bool isDead() override;
    void TakeHealth(int iRemoval) override;
    void AddHealth(int iAdd) override;
    void rest(World& world);
    void printHealth() override;
    int getHealth() const { return iHealth; }
    void setHealth(int h);
};
