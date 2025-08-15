#pragma once
#include <vector>
#include <string>
#include <iostream> // needed for cout

using namespace std;

class Menu
{
private:
    int selectedIndex = 0;
    vector<string> menuItems;
public:
    Menu(vector<string> items);

    void DisplayItems();
    void MoveDown();
    void MoveUp();
    string returnSelected();
};
