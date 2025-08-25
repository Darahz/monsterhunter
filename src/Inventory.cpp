#include "../include/Inventory.h"
#include "../include/Utils.h"
#include <iostream>

Inventory::Inventory() {}

void Inventory::addItem(const Item& item, int num) {
    std::string name = toFirstUpper(item.name); // normalize
    if (this->items.count(name)) {
        this->items[name].second += num;
    } else {
        this->items[name] = std::make_pair(item, num);
    }
}

void Inventory::listItems() const {
    if (items.empty()) {
        std::cout << "Inventory is empty!" << std::endl;
        return;
    }
    for (const auto &kv : items) {
        const Item& item = kv.second.first;
        int count = kv.second.second;
        std::cout << item.name << ": x" << count;
        if (!item.description.empty()) std::cout << " - " << item.description;
        std::cout << std::endl;
    }
}

int Inventory::getItemCount(const std::string& name) const {
    std::string key = toFirstUpper(name);
    if (this->items.count(key)) {
        return this->items.at(key).second;
    }
    return 0;
}

bool Inventory::hasItem(const std::string& name) const {
    std::string key = toFirstUpper(name);
    return this->items.count(key) > 0 && this->items.at(key).second > 0;
}

void Inventory::removeItem(const std::string& name, int amt) {
    std::string key = toFirstUpper(name);
    if (!items.count(key)) return;
    items[key].second -= amt;
    if (items[key].second <= 0) {
        items.erase(key);
        if (toFirstUpper(equipped) == key) equipped.clear(); // auto-unequip if gone
    }
}

std::vector<std::pair<Item, int>> Inventory::getItemsList() const {
    std::vector<std::pair<Item, int>> out;
    out.reserve(items.size());
    for (const auto &kv : items) {
        out.emplace_back(kv.second.first, kv.second.second);
    }
    return out;
}

bool Inventory::equipItem(const std::string& name) {
    std::string key = toFirstUpper(name);
    auto it = items.find(key);
    if (it == items.end() || it->second.second <= 0) return false;
    equipped = key; // normalize to lowercase internally
    return true;
}

void Inventory::clearInventory(){
    this->items.clear();
}