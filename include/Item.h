#include <string>


#pragma once
#include <string>

class Item {
public:
    std::string name;
    std::string description;
    int value;
    bool equipable;
    bool edible;

    Item() : name(""), description(""), value(0), equipable(false), edible(false) {}
    Item(const std::string& n, const std::string& desc, int val, bool eq, bool ed = false)
        : name(n), description(desc), value(val), equipable(eq), edible(ed) {}
};
