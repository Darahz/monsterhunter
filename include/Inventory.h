#pragma once
#include <vector>
#include "Item.h"

class Inventory {
public:
    Inventory();
    ~Inventory();

    void addItem(const Item& item);
    void removeItem(const Item& item);
    void clear();

    std::vector<Item> getItems() const;

private:
    std::vector<Item> items;
};
