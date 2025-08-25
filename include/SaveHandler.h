#pragma once
#include <string>
#include <vector>
#include "Player.h"
#include "World.h"

class SaveHandler {
public:
    static bool saveGame(const Player& player, const World& world, const std::string& saveName);
    static bool loadGame(Player& player, World& world, const std::string& saveName);
    static std::vector<std::string> getSaveFiles();
    static bool deleteSave(const std::string& saveName);
    
    // Active save management
    static void setActiveSave(const std::string& saveName);
    static std::string getActiveSave();
    static bool hasActiveSave();
    static bool saveToActiveSave(const Player& player, const World& world);
    
private:
    static std::string getSaveDirectory();
    static std::string getSaveFilePath(const std::string& saveName);
    static void createSaveDirectory();
    
    static std::string activeSaveName;
};
