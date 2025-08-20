#include "../include/Inventory.h"

Inventory::Inventory() {}

Inventory::~Inventory() {}

void Inventory::drawInventory() {
    for (const auto& item : items) {
        std::cout << "Drawing item: " << item.getName() << " with value: " << item.getValue() << std::endl;
    }
}

void Inventory::addItem(const Item& item) {
    items.push_back(item);
}

void Inventory::removeItem(const Item& item) {
    items.erase(std::remove_if(items.begin(), items.end(),
        [&item](const Item& i) { return i.getName() == item.getName(); }), items.end());
}

void Inventory::clear() {
    items.clear();
}

std::vector<Item> Inventory::getItems() const {
    return items;
}
