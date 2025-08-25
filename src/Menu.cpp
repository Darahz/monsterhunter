#include "../include/Menu.h"
#include <iostream>

Menu::Menu(vector<string> items) {
    this->menuItems = items;
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
