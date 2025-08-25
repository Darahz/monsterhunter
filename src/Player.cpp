#include "../include/Player.h"
#include "../include/Utils.h"
#include "../include/Recipes.h"
#include "../include/World.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <termios.h>
#include <unistd.h>
#include <algorithm>

Player::Player() {}

bool Player::craftItem(std::string itemName){
    auto it = RECIPES.find(itemName);
    if (it == RECIPES.end()) {
        std::cout << "You can't craft '" << itemName << "'." << std::endl;
        return false;
    }
    const auto &needs = it->second;

    // Check if we have all ingredients
    for (const auto &req : needs) {
        const std::string &reqName = req.first;
        int reqAmt = req.second;
        int have = inventory.getItemCount(reqName);
        if (have < reqAmt) {
            std::cout << "Missing " << reqName << " x" << (reqAmt - have) << std::endl;
            return false;
        }
    }

    // Consume ingredients
    for (const auto &req : needs) {
        const std::string &reqName = req.first;
        int reqAmt = req.second;
        inventory.removeItem(reqName, reqAmt);
    }

    // Give the crafted item (1 by default)
    inventory.addItem(itemName, 1);
    std::cout << "Crafted: " << itemName << "!" << std::endl;
    return true;
}


void Player::rest(World& world){
    ClearScreen();

    if (hasItem("Apple")){
        Print("You ate an apple before bedtime");
        AddHealth(15);
        removeItem("Apple");
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
    AddHealth(30);

    tcflush(STDIN_FILENO, TCIFLUSH);
    Print("Press any key to continue...");
    getchar();
}

bool Player::isDead(){
    return iHealth <= 0;
}

void Player::TakeHealth(int iRemoval){
    if(iHealth - iRemoval <= 0){
        Print("You died...");
    }else{
        iHealth -= iRemoval;
        int curHealth = iHealth;
        Print(std::string("You've lost ") + std::to_string(iRemoval) + std::string(" health ") + std::to_string(curHealth) + std::string("/100"), Color::Red);
    }
}

void Player::AddHealth(int iAdd){
    iHealth += iAdd;
    if (iHealth > 100) iHealth = 100;
    Print("You've gained " + std::to_string(iAdd) +
        " health " + std::to_string(iHealth) + "/100", Color::Green);
}

void Player::printHealth(){
    if(iHealth >= 75){
        Print("Health : " + std::to_string(iHealth), Color::Green);
    } else if(iHealth >= 25){
        Print("Health : " + std::to_string(iHealth), Color::Yellow);
    } else {
        Print("Health : " + std::to_string(iHealth), Color::Red);
    }
}

void Player::setHealth(int h) {
    iHealth = std::max(0, std::min(100, h));
}

void Player::clearInventory() {
    inventory.clearInventory();
}