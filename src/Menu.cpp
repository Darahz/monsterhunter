
#include "../include/Menu.h"
#include <iostream>
#include <functional>


Menu::Menu(vector<string> items) {
    this->menuItems = items;
    this->hasCallbacks = false;
}

Menu::Menu(vector<pair<string, function<void()>>> itemsWithCallbacks) {
    this->menuItems.reserve(itemsWithCallbacks.size());
    this->callbacks.reserve(itemsWithCallbacks.size());
    for (auto &p : itemsWithCallbacks) {
        this->menuItems.push_back(p.first);
        this->callbacks.push_back(p.second);
    }
    this->hasCallbacks = true;
}
void Menu::Select() {
    if (hasCallbacks && selectedIndex >= 0 && selectedIndex < (int)callbacks.size()) {
        callbacks[selectedIndex]();
    }
}

void Menu::DisplayItems() {
    int index = 0;
    for (const auto &item : this->menuItems) {
        if (index == this->selectedIndex) {
            cout << " >" << item << endl;
        } else {
            cout << item << endl;
        }
        index++;
    }
}

void Menu::MoveDown() {
    this->selectedIndex++;
    if (this->selectedIndex >= (int)this->menuItems.size()) {
        this->selectedIndex = 0;
    }
}

void Menu::MoveUp() {
    this->selectedIndex--;
    if (this->selectedIndex < 0) {
        this->selectedIndex = (int)this->menuItems.size() - 1;
    }
}

string Menu::returnSelected() {
    return this->menuItems[this->selectedIndex];
}
