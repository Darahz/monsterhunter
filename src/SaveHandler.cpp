#include <cstdint>
#include <string>
#include <vector>
#include "../include/Player.h"

#include "../include/SaveHandler.h"
#include "../include/Utils.h"
#include <fstream>
#include <filesystem>
#include <ctime>
#include <sstream>

std::string SaveHandler::activeSaveName = "";

std::string SaveHandler::getSaveDirectory() {
    return ".saves";
}

std::string SaveHandler::getSaveFilePath(const std::string& saveName) {
    return getSaveDirectory() + "/" + saveName + ".save";
}

void SaveHandler::createSaveDirectory() {
    std::filesystem::create_directories(getSaveDirectory());
}

bool SaveHandler::saveGame(const Player& player, const World& world, const std::string& saveName) {
    createSaveDirectory();
    std::string filePath = getSaveFilePath(saveName);
    std::ostringstream raw;
    raw << "HEALTH:" << player.getHealth() << "\n";
    raw << "EQUIPPED:" << player.getEquipped() << "\n";
    raw << "WORLD_HOUR:" << world.getHour() << "\n";
    raw << "WORLD_MINUTE:" << world.getMinute() << "\n";
    raw << "WORLD_DAY:" << world.getDay() << "\n";
    auto inventory = player.getInventoryList();
    raw << "INVENTORY_START\n";
    for (const auto& item : inventory) {
        raw << item.first.name << ":" << item.second << "\n";
    }
    raw << "INVENTORY_END\n";
    std::string data = raw.str();
    xorObfuscate(data);
    uint32_t checksum = calcChecksum(data);
    std::ofstream file(filePath, std::ios::binary);
    if (!file.is_open()) {
        Print("Failed to create save file!", Color::Red);
        return false;
    }
    file.write(data.data(), data.size());
    file.write(reinterpret_cast<const char*>(&checksum), sizeof(checksum));
    file.close();
    setActiveSave(saveName);
    Print("Game saved as: " + saveName, Color::Green);
    return true;
}

bool SaveHandler::loadGame(Player& player, World& world, const std::string& saveName) {
    std::string filePath = getSaveFilePath(saveName);
    std::ifstream file(filePath, std::ios::binary);
    if (!file.is_open()) {
        Print("Failed to load save file: " + saveName, Color::Red);
        return false;
    }
    file.seekg(0, std::ios::end);
    std::streampos fileSize = file.tellg();
    if (fileSize < (std::streampos)sizeof(uint32_t)) {
        Print("Corrupt save file: " + saveName, Color::Red);
        return false;
    }
    file.seekg(0, std::ios::beg);
    std::string data((size_t)fileSize - sizeof(uint32_t), '\0');
    file.read(&data[0], data.size());
    uint32_t fileChecksum = 0;
    file.read(reinterpret_cast<char*>(&fileChecksum), sizeof(fileChecksum));
    file.close();
    uint32_t actualChecksum = calcChecksum(data);
    if (fileChecksum != actualChecksum) {
        Print("Save file tampered or corrupt: " + saveName, Color::Red);
        return false;
    }
    xorObfuscate(data);
    std::istringstream in(data);
    world.stop();
    int health = 100;
    std::string equipped;
    int wHour = 8, wMinute = 0, wDay = 1;
    player.clearInventory();
    std::string line;
    bool inInventory = false;
    auto safe_stoi = [](const std::string& s, int fallback = 0) {
        try { return std::stoi(s); } catch (...) { return fallback; }
    };
    while (std::getline(in, line)) {
        if (line == "INVENTORY_START") { inInventory = true; continue; }
        if (line == "INVENTORY_END")   { inInventory = false; continue; }
        size_t colonPos = line.find(':');
        if (colonPos == std::string::npos) continue;
        std::string key   = line.substr(0, colonPos);
        std::string value = line.substr(colonPos + 1);
        if (inInventory) {
            int count = safe_stoi(value, 0);
            if (count > 0) player.addItem(key, count);
        } else {
            if (key == "HEALTH") {
                health = safe_stoi(value, 100);
            } else if (key == "EQUIPPED") {
                equipped = value;
            } else if (key == "WORLD_HOUR") {
                wHour = safe_stoi(value, 8);
            } else if (key == "WORLD_MINUTE") {
                wMinute = safe_stoi(value, 0);
            } else if (key == "WORLD_DAY") {
                wDay = safe_stoi(value, 1);
            }
        }
    }
    player.setHealth(health);
    if (!equipped.empty()) {
        player.equipItem(equipped);
    }
    world.setDay(wDay);
    world.setTime(wHour, wMinute);
    setActiveSave(saveName);
    Print("Game loaded: " + saveName, Color::Green);
    world.start();
    return true;
}


std::vector<std::string> SaveHandler::getSaveFiles() {
    std::vector<std::string> saveFiles;
    
    if (!std::filesystem::exists(getSaveDirectory())) {
        return saveFiles;
    }
    
    for (const auto& entry : std::filesystem::directory_iterator(getSaveDirectory())) {
        if (entry.is_regular_file() && entry.path().extension() == ".save") {
            std::string filename = entry.path().stem().string();
            saveFiles.push_back(filename);
        }
    }
    
    return saveFiles;
}

bool SaveHandler::deleteSave(const std::string& saveName) {
    std::string filePath = getSaveFilePath(saveName);
    
    if (std::filesystem::exists(filePath)) {
        std::filesystem::remove(filePath);
        
        if (activeSaveName == saveName) {
            activeSaveName = "";
        }
        
        Print("Save deleted: " + saveName, Color::Yellow);
        return true;
    }
    
    Print("Save file not found: " + saveName, Color::Red);
    return false;
}

void SaveHandler::setActiveSave(const std::string& saveName) {
    activeSaveName = saveName;
}

std::string SaveHandler::getActiveSave() {
    return activeSaveName;
}

bool SaveHandler::hasActiveSave() {
    return !activeSaveName.empty();
}

bool SaveHandler::saveToActiveSave(const Player& player, const World& world) {
    if (!hasActiveSave()) {
        Print("No active save set!", Color::Red);
        return false;
    }
    
    return saveGame(player, world, activeSaveName);
}

void SaveHandler::xorObfuscate(std::string& data) {
    const char key = 0x5A;
    for (char& c : data) {
        c ^= key;
    }
}

uint32_t SaveHandler::calcChecksum(const std::string& data) {
    uint32_t sum = 0;
    for (unsigned char c : data) sum += c;
    return sum;
}
