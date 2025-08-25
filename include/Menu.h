
#pragma once
#include <vector>
#include <string>
#include <functional>

using namespace std;

class Menu
{
private:
    int selectedIndex = 0;
    vector<string> menuItems;
    vector<function<void()>> callbacks;
    bool hasCallbacks = false;
public:
    Menu(vector<string> items);
    Menu(vector<pair<string, function<void()>>> itemsWithCallbacks);

    void DisplayItems();
    void MoveDown();
    void MoveUp();
    string returnSelected();
    void Select();
};
