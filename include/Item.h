#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>


class Item {
public:
    Item(const std::string& name, int value);
    ~Item();

    std::string getName() const;
    int getValue() const;

private:
    std::string name;
    int value;
};
