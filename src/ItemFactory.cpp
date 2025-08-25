#include "../include/ItemFactory.h"
#include "../include/Utils.h"

Item getItemByName(const std::string& name) {
    std::string n = toFirstUpper(name);
    if (n == "Wood") return Item("Wood", "A piece of wood. Useful for crafting and building structures.", 1, false, false);
    if (n == "Apple") return Item("Apple", "A juicy apple. Restores a little health when eaten.", 2, false, true);
    if (n == "Axe") return Item("Axe", "A sharp axe for chopping wood. Essential for gathering resources efficiently.", 20, true, false);
    if (n == "Axe Handle") return Item("Axe handle", "A wooden handle for an axe. Can be used alone, but not very effective.", 5, true, false);
    if (n == "Stone") return Item("Stone", "A small stone. Useful for crafting tools and structures.", 1, false, false);
    if (n == "Stack Of Apples") return Item("Stack of apples", "A bundle of apples. Great for restoring health over time.", 10, false, true);
    if (n == "Campfire") return Item("Campfire", "A simple campfire. Lets you rest and recover health at night.", 15, false, false);
    if (n == "Sword Handle") return Item("Sword handle", "A handle for a sword. Used in advanced weapon crafting.", 7, true, false);
    // Add more items as needed
    return Item(n, "A mysterious item with unknown properties.", 0, false, false);
}