#pragma once
#include <unordered_map>
#include <string>

// Recipe names will be normalized with toFirstUpper() in the inventory
using Recipe = std::unordered_map<std::string,int>;

inline const std::unordered_map<std::string, Recipe> RECIPES = {
    {"stack of apples", { {"apple", 5} }},
    {"axe",             { {"wood", 3}, {"stone", 1} }},
    {"campfire",        { {"wood", 2}, {"stone", 2} }},
    {"axe handle",      { {"wood", 2} }},
    {"sword handle",    { {"wood", 2} }},
};
