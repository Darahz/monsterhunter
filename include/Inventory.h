#pragma once
#include <string>
#include <unordered_map>
#include <vector>

class Inventory {
private:
    std::unordered_map<std::string,int> items;
    std::string equipped;

public:
    Inventory();
    void addItem(std::string name, int num);
    void removeItem(std::string name, int amt = 1);
    void listItems() const;
    
    int getItemCount(std::string name) const;
    bool hasItem(std::string name) const;
    
    std::vector<std::pair<std::string,int>> getItemsList() const;
    
    bool equipItem(std::string name);
    std::string getEquipped() const { return equipped; }
};
