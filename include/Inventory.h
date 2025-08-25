#pragma once
#include <string>

#include <unordered_map>
#include <vector>
#include "Item.h"

class Inventory {
private:
    // Map item name to (Item object, count)
    std::unordered_map<std::string, std::pair<Item, int>> items;
    std::string equipped;

public:
    Inventory();
    // Add an item (by Item object) and count
    void addItem(const Item& item, int num);
    // Remove by name (for now)
    void removeItem(const std::string& name, int amt = 1);
    void listItems() const;
    void clearInventory();

    int getItemCount(const std::string& name) const;
    bool hasItem(const std::string& name) const;

    // Return vector of (Item, count)
    std::vector<std::pair<Item, int>> getItemsList() const;

    bool equipItem(const std::string& name);
    std::string getEquipped() const { return equipped; }
};
