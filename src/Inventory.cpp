#include "../include/Inventory.h"
#include "../include/Utils.h"
#include <iostream>

Inventory::Inventory() {}

void Inventory::addItem(std::string name, int num) {
    name = toFirstUpper(name); // normalize
    if(this->items.count(name)){
        this->items[name] += num;
    } else {
        this->items[name] = num;
    }
}

void Inventory::listItems() const {
    if (items.empty()) {
        std::cout << "Inventory is empty!" << std::endl;
        return;
    }
    for (const auto &item : items) {
        std::cout << item.first << ": " << item.second << std::endl;
    }
}

int Inventory::getItemCount(std::string name) const {
    name = toFirstUpper(name); // normalize
    if(this->items.count(name)){
        return this->items.at(name);
    }
    return 0;
}

bool Inventory::hasItem(std::string name) const {
    name = toFirstUpper(name); // normalize
    return this->items.count(name) > 0 && this->items.at(name) > 0;
}

void Inventory::removeItem(std::string name, int amt) {
    name = toFirstUpper(name);
    if (!items.count(name)) return;
    items[name] -= amt;
    if (items[name] <= 0) {
        items.erase(name);
        if (toFirstUpper(equipped) == name) equipped.clear(); // auto-unequip if gone
    }
}

std::vector<std::pair<std::string,int>> Inventory::getItemsList() const {
    std::vector<std::pair<std::string,int>> out;
    out.reserve(items.size());
    for (const auto &kv : items) {
        out.emplace_back(kv.first, kv.second);
    }
    return out;
}

bool Inventory::equipItem(std::string name) {
    std::string key = toFirstUpper(name);
    auto it = items.find(key);
    if (it == items.end() || it->second <= 0) return false;
    equipped = key; // normalize to lowercase internally
    return true;
}

void Inventory::clearInventory(){
    this->items.clear();
}