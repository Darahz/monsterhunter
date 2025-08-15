#include "../include/Player.h"
#include "../include/Utils.h"
#include "../include/Recipes.h"
#include "../include/World.h"

Player::Player() {}

void Player::addItem(std::string name, int num){
    name = toLower(name); // normalize
    if(this->inventory.count(name)){
        this->inventory[name] += num;
    } else {
        this->inventory[name] = num;
    }
}

void Player::listInventory() {
    if (inventory.empty()) {
        std::cout << "Inventory is empty!" << std::endl;
        return;
    }
    for (const auto &item : inventory) {
        std::cout << item.first << ": " << item.second << std::endl;
    }
}

int Player::getItemCount(std::string name){
    name = toLower(name); // normalize
    if(this->inventory.count(name)){
        return this->inventory[name];
    }
    return 0;
}

bool Player::hasItem(std::string name){
    name = toLower(name); // normalize
    return this->inventory.count(name) > 0 && this->inventory.at(name) > 0;
}

void Player::removeItem(std::string name, int amt) {
    name = toLower(name);
    if (!inventory.count(name)) return;
    inventory[name] -= amt;
    if (inventory[name] <= 0) {
        inventory.erase(name);
        if (toLower(equipped) == name) equipped.clear(); // auto-unequip if gone
    }
}

std::vector<std::pair<std::string,int>> Player::getInventoryList() const {
    std::vector<std::pair<std::string,int>> out;
    out.reserve(inventory.size());
    for (const auto &kv : inventory) {
        out.emplace_back(kv.first, kv.second);
    }
    return out;
}

bool Player::equipItem(std::string name) {
    std::string key = toLower(name);
    auto it = inventory.find(key);
    if (it == inventory.end() || it->second <= 0) return false;
    equipped = key; // normalize to lowercase internally
    return true;
}

bool Player::craftItem(std::string itemName){
    auto it = RECIPES.find(itemName);
    if (it == RECIPES.end()) {
        std::cout << "You can't craft '" << itemName << "'." << std::endl;
        return false;
    }
    const auto &needs = it->second;

    for (const auto &req : needs) {
        const std::string &reqName = req.first;
        int reqAmt = req.second;
        int have = getItemCount(reqName);
        if (have < reqAmt) {
            std::cout << "Missing " << reqName << " x" << (reqAmt - have) << std::endl;
            return false;
        }
    }

    // 3) Consume ingredients
    for (const auto &req : needs) {
        const std::string &reqName = req.first;
        int reqAmt = req.second;

        // Safe subtract: we know the key exists & count is enough
        inventory[reqName] -= reqAmt;
        if (inventory[reqName] <= 0) {
            inventory.erase(reqName);
        }
    }

    // 4) Give the crafted item (1 by default)
    addItem(itemName, 1);
    std::cout << "Crafted: " << itemName << "!" << std::endl;
    return true;
}


void Player::rest(World& world){
    ClearScreen();

    if (this->hasItem("Apple")){
        Print("You ate an apple before bedtime");
        this->AddHealth(15);
        this->removeItem("Apple");
    } else {
        Print("You don't have an apple");
    }
    Print("You set up a small camp and closes your eyes");
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    // Figure out how many hours to jump to the *next* 08:00
    int h = world.getHour();
    int to08 = (h < 8) ? (8 - h) : ((24 - h) + 8);  // always >= 0

    Print("You're resting now... (until 08:00)");
    for (int i = 0; i < to08; ++i) {
        // Optional: show time before/after advancing
        Print("Time: " + world.timeString());
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        world.addHour(1);
        ClearScreen();
        Print("You're resting now... (until 08:00)");
    }

    Print("You wake up at 08:00 feeling rested!");
    this->AddHealth(30);

    tcflush(STDIN_FILENO, TCIFLUSH);
    Print("Press any key to continue...");
    getchar();
}



bool Player::isDead(){
    return this->iHealth <= 0;
}

void Player::TakeHealth(int iRemoval){
    if(this->iHealth - iRemoval <= 0){
        Print("You died...");
    }else{
        this->iHealth -= iRemoval;
        int curHealth = this->iHealth;
        Print(std::string("You've lost ") + std::to_string(iRemoval) + std::string(" health ") + std::to_string(curHealth) + std::string("/100"), Color::Red);
    }
}

void Player::AddHealth(int iAdd){
    this->iHealth += iAdd;
    if (this->iHealth > 100) this->iHealth = 100;
    Print("You've gained " + std::to_string(iAdd) +
        " health " + std::to_string(this->iHealth) + "/100", Color::Green);
}