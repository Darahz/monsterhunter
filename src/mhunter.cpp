#include <vector>
#include <iostream>
#include <string>
#include <termios.h>
#include <unistd.h>
#include <unordered_map>
#include <functional>
#include <ctime>

#include <thread>
#include <chrono>

#include "../include/Player.h"
#include "../include/Utils.h"
#include "../include/Recipes.h"
#include "../include/Menu.h"
#include "../include/World.h"
#include "../include/SaveHandler.h"


using namespace std;

struct RawMode {
    termios oldt{};
    bool ok = false;

    RawMode() {
        if (tcgetattr(STDIN_FILENO, &oldt) == 0) {
            termios newt = oldt;
            // Turn off canonical mode (ICANON) and echo (ECHO)
            newt.c_lflag &= ~(ICANON | ECHO);
            // Read returns after 1 byte, no timeout
            newt.c_cc[VMIN]  = 1;
            newt.c_cc[VTIME] = 0;
            if (tcsetattr(STDIN_FILENO, TCSANOW, &newt) == 0) ok = true;
        }
    }
    ~RawMode() {
        if (ok) tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    }
};

void ChopWood(Player &_player){
    bool hasAxe       = _player.getEquipped() == "Axe";
    bool hasAxeHandle = _player.getEquipped() == "Axe handle";
    int totalWood = 0;

    if (hasAxe) {
        // Axe: efficient chopping
        int rndNum = getRandomNum(2,5);
        for (int i = 0; i < rndNum; i++) {
            int numLumber = getRandomNum(2,4);
            Print("You swing the axe!");
            PrintDot(3);
            Print(std::string("You've gained ") + std::to_string(numLumber) + " wood!");
            sleep(1);
            totalWood += numLumber;
        }
    }
    else if (hasAxeHandle) {
        // Axe Handle: works, but worse than a full axe
        int rndNum = getRandomNum(1,5);
        for (int i = 0; i < rndNum; i++) {
            int numLumber = getRandomNum(1,3);
            Print("You bonked the tree with the Axe Handle!");
            PrintDot(3);
            if(getRandomNum(1,100) <= 15){
                Print("The handle kicked back and hit you in the face... Ouch", Color::Red);
                _player.TakeHealth(getRandomNum(1,4));
            }
            sleep(1);
            Print(std::string("You've gained ") + std::to_string(numLumber) + " wood!");
            sleep(1);
            totalWood += numLumber;
        }
    }
    else {
        // Bare hands (or head): prompt + worst yields
        bool invHasAxe = _player.hasItem("Axe");
        string axeMsg = "Do you want to use your bare hands to chop wood?";
        if (invHasAxe) {
            axeMsg = "You have a axe. But you haven't equipped it. Do you want to use your bare hands to chop wood?";
        }
        bool yn = PrintYesNo(axeMsg);
        if (!yn) return;
        ClearScreen();
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        Print("You decided to continue with your plan!... To get that wood");
        PrintDot(3, 250);
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));

        int rndNum = getRandomNum(1,5);
        for (int i = 0; i < rndNum; i++) {
            int numLumber = getRandomNum(1,3);
            Print("You slammed your head into the tree!");
            PrintDot(3);
            _player.TakeHealth(getRandomNum(1,3));
            sleep(1);
            Print(std::string("You've gained ") + std::to_string(numLumber) + " wood!");
            sleep(1);
            totalWood += numLumber;
        }
    }

    ClearScreen();
    _player.addItem("Wood", totalWood);
    Print(std::string("Managed to get ") + std::to_string(totalWood) + "x wood.");
    if(getRandomNum(1,100) <= 35){
        _player.addItem("Apple", totalWood);
        Print(std::string("And ") + std::to_string(5) + "x Apples.");
    }
    Print("Press any key…"); getchar();
}

void OpenCraftingMenu(Player &_player) {
    // Extract recipe names into a vector
    std::vector<std::string> craftables;
    for (const auto &r : RECIPES) {
        craftables.push_back(r.first);
    }
    craftables.push_back("Back"); // to exit the crafting menu

    Menu craftMenu(craftables);

    while (true) {
        ClearScreen();
        std::cout << "== Crafting Menu ==" << std::endl;
        craftMenu.DisplayItems();

        int ch = getchar();
        switch (ch) {
            case 'w': case 'W':
                craftMenu.MoveUp();
                break;
            case 's': case 'S':
                craftMenu.MoveDown();
                break;
            case 'e': {
                std::string choice = craftMenu.returnSelected();
                if (choice == "Back") {
                    return; // go back to main menu
                }
                ClearScreen();
                if (_player.craftItem(choice)) {
                    std::cout << "Crafted " << choice << "!" << std::endl;
                } else {
                    std::cout << "Failed to craft " << choice << "!" << std::endl;
                }
                Print("Press any key…"); getchar();
                break;
            }
            case 'q': case 'Q':
                return;
        }
    }
}

static std::vector<std::string> BuildInventoryItems(Player& _player) {
    auto list = _player.getInventoryList(); // vector<pair<string,int>>
    std::vector<std::string> items;
    items.reserve(list.size() + 1);
    for (auto &p : list) {
        items.push_back(p.first + " x" + std::to_string(p.second));
    }
    items.push_back("Back");
    return items;
}

void OpenInventoryMenu(Player &_player) {
    auto items = BuildInventoryItems(_player);
    Menu invMenu(items);

    while (true) {
        ClearScreen();
        Print("== Inventory ==");
        if (!_player.getEquipped().empty())
            Print(std::string("Equipped: ") + _player.getEquipped());
        Print("");

        invMenu.DisplayItems();

        int ch = getchar();
        switch (ch) {
            case 'w': case 'W':
                invMenu.MoveUp();
                break;

            case 's': case 'S':
                invMenu.MoveDown();
                break;

            case 'e': {
                std::string sel = invMenu.returnSelected();
                if (sel == "Back") return;

                auto pos = sel.find(" x");
                std::string itemName = (pos == std::string::npos) ? sel : sel.substr(0, pos);

                ClearScreen();
                Print(std::string("Selected: ") + itemName);
                Print("E = Equip, D = Drop 1, B = Back");

                int a = getchar();
                bool changed = false;

                if (a == 'E' || a == 'e') {
                    if (_player.equipItem(itemName)) {
                        Print(std::string("Equipped: ") + itemName, Color::Green);
                    } else {
                        Print("You don't have that item.", Color::Red);
                    }
                    Print("Press any key…"); getchar();
                } else if (a == 'D' || a == 'd') {
                    if (_player.getItemCount(itemName) > 0) {
                        _player.removeItem(itemName, 1);
                        Print(std::string("Dropped 1 ") + itemName, Color::Yellow);
                        changed = true;
                    } else {
                        Print("You don't have that item.", Color::Red);
                    }
                    Print("Press any key…"); getchar();
                }
                if (changed) {
                    int oldIdx = 0;

                    oldIdx = 0;

                    items = BuildInventoryItems(_player);
                    Menu newMenu(items);

                    for (int i = 0; i < oldIdx && i < (int)items.size(); ++i) newMenu.MoveDown();
                    invMenu = std::move(newMenu);
                }
                break;
            }

            case 'q': case 'Q':
            case 'b': case 'B':
                return;

            default:
                break;
        }
    }
}

void OpenExplorationMenu(Player &_player, World &_world){
    Menu exploreMenu({"Fight", "Chop wood", "Look around"});
    ClearScreen();
    unordered_map<string, function<void()>> actions{
        {"Look around", [&]{
            int rndNum = getRandomNum(1,5);
            if(rndNum <= 2){
                Print("You look around");
                PrintDot(3);
                Print("You see nothing out of the ordinary. But you did spot a nice shiny stone!");
                _player.addItem("Stone", getRandomNum(1,2));
                if(getRandomNum(0,100) <= 25){
                    PrintDot(3,250);
                    Print("It's really sharp!");
                    PrintDot(3);
                    _player.TakeHealth(2);
                }
            }else{
                PrintDot(3,250);
                Print("You see a big lake in front of you. The birds are singing a beautiful song");
                PrintDot(3,250);
            }
            Print("Press any key…"); getchar();
        }},
        {"Chop wood", [&]{
            ChopWood(_player);
        }},
    };
    while (true) {
        ClearScreen();
        Print("== Exploration ==");
        Print(_world.timeString());
        Print("");
        exploreMenu.DisplayItems();
        int ch = getchar();
        switch (ch) {
            case 'w': case 'W':
                exploreMenu.MoveUp();
                break;
            case 's': case 'S':
                exploreMenu.MoveDown();
                break;
            case 'e':{
                auto it = actions.find(exploreMenu.returnSelected());
                if (it != actions.end()) it->second();
                break;
            }
            case 'q': case 'Q':
                return;
            default:
                break;
        }
        if (_world.isNight()) {

        }
    }
}

void OpenPlayerMenu(Player &_player, World &_world){
    Menu playerMenu({"Inventory", "Equipped Item", "Craft", "Rest"});
    ClearScreen();
    unordered_map<string, function<void()>> actions{
        {"Inventory", [&]{
            OpenInventoryMenu(_player);
        }},
        {"Equipped Item", [&]{
            ClearScreen();
            Print("== Equipped Item ==");
            if (_player.getEquipped().empty()) {
                Print("You have no item equipped.", Color::Red);
            }else {
                Print("You have equipped: " + _player.getEquipped(), Color::Green);
            }
            Print("Press any key…"); getchar();
        }},
        {"Craft", [&]{
            OpenCraftingMenu(_player);
        }},
        {"Rest", [&]{
            if(PrintYesNo("This will forward the time to 08:00 next day, do you want to continue?")){
                _player.rest(_world);
            }
        }},
    };
    while (true) {
        ClearScreen();
        Print("== Player ==");
        Print(_world.timeString());
        Print("");
        playerMenu.DisplayItems();
        int ch = getchar();
        switch (ch) {
            case 'w': case 'W':
                playerMenu.MoveUp();
                break;
            case 's': case 'S':
                playerMenu.MoveDown();
                break;
            case 'e':{
                auto it = actions.find(playerMenu.returnSelected());
                if (it != actions.end()) it->second();
                break;
            }
            case 'q': case 'Q':
                return;
            default:
                break;
        }
    }
}

void OpenLoadMenu(Player &_player, World &_world) {
    auto saveFiles = SaveHandler::getSaveFiles();
    if (saveFiles.empty()) {
        ClearScreen();
        Print("No save files found!", Color::Yellow);
        Print("Press any key to continue...");
        getchar();
        return;
    }
    
    saveFiles.push_back("Back");
    Menu loadMenu(saveFiles);
    
    while (true) {
        ClearScreen();
        Print("== Load Game ==");
        if (SaveHandler::hasActiveSave()) {
            Print("Current active save: " + SaveHandler::getActiveSave(), Color::Cyan);
        }
        Print("Select a save file to load:");
        Print("");
        loadMenu.DisplayItems();
        
        int ch = getchar();
        switch (ch) {
            case 'w': case 'W':
                loadMenu.MoveUp();
                break;
            case 's': case 'S':
                loadMenu.MoveDown();
                break;
            case 'e': {
                std::string selected = loadMenu.returnSelected();
                if (selected == "Back") return;
                
                if (PrintYesNo("Load save: " + selected + "? Current progress will be lost!")) {
                    if (SaveHandler::loadGame(_player, _world, selected)) {
                        Print("Save loaded successfully!");
                        Print("This is now your active save file.", Color::Cyan);
                        Print("Press any key to continue...");
                        getchar();
                        return;
                    }
                }
                break;
            }
            case 'q': case 'Q':
                return;
        }
    }
}

void SaveHandlingMenu(Player &_player, World &_world){
    _world.stop();
    
    std::vector<std::string> menuItems;
    if (SaveHandler::hasActiveSave()) {
        menuItems = {"Quick Save (" + SaveHandler::getActiveSave() + ")", "Save Game", "Load Game", "Back"};
    } else {
        menuItems = {"Save Game", "Load Game", "Back"};
    }
    
    Menu SaveMenu(menuItems);
    
    unordered_map<string, function<void()>> actions{
        {"Quick Save (" + SaveHandler::getActiveSave() + ")", [&]{
            ClearScreen();
            Print("Saving to: " + SaveHandler::getActiveSave());
            if (SaveHandler::saveToActiveSave(_player, _world)) {
                Print("Press any key to continue...");
                getchar();
            }
        }},
        {"Save Game", [&]{
            ClearScreen();
            Print("Enter save name (or press Enter for default):");
            
            // Get current time for default name
            time_t now = time(0);
            tm* ltm = localtime(&now);
            char timeStr[100];
            sprintf(timeStr, "Save_%04d%02d%02d_%02d%02d%02d", 
                    1900 + ltm->tm_year, 1 + ltm->tm_mon, ltm->tm_mday,
                    ltm->tm_hour, ltm->tm_min, ltm->tm_sec);
            
            std::string saveName = timeStr;
            Print("Save name: " + saveName);
            
            if (SaveHandler::saveGame(_player, _world, saveName)) {
                Print("This is now your active save file.");
                Print("Press any key to continue...");
                getchar();
            }
        }},
        {"Load Game", [&]{
            OpenLoadMenu(_player, _world);
        }},
        {"Back", [&]{
            // Do nothing, will exit loop
        }}
    };
    
    while (true) {
        ClearScreen();
        Print("== Data Management ==");
        Print(_world.timeString());
        _player.printHealth();
        Print("");
        SaveMenu.DisplayItems();
        
        int ch = getchar();
        switch (ch) {
            case 'w': case 'W':
                SaveMenu.MoveUp();
                break;
            case 's': case 'S':
                SaveMenu.MoveDown();
                break;
            case 'e':{
                std::string selected = SaveMenu.returnSelected();
                if (selected == "Back") {
                    _world.start();
                    return;
                }
                auto it = actions.find(selected);
                if (it != actions.end()) it->second();
                break;
            }
            case 'q': case 'Q':
                _world.start();
                return;
            default:
                break;
        }
    }
}

int main(){
    World _world;
    Player _player;
    _world.start();

    std::string title = "MonsterHunter v.0.0.1";
    std::cout << "\033]0;" << title << "\007";
    HideCursor();
    ClearScreen();
    Menu menu({
        "Explore",
        "Player",
        "Data",
        "Exit"});
    Print(_world.timeString());
    _player.printHealth();
    Print("");
    menu.DisplayItems();

    
    unordered_map<string, function<void()>> actions{
        {"Explore", [&]{
            OpenExplorationMenu(_player, _world);
        }},
        {"Player", [&]{
            OpenPlayerMenu(_player, _world);
        }},{"Data" , [&]{
            SaveHandlingMenu(_player, _world);
        }},
        {"Exit", [&]{
            ShowCursor();
            std::exit(EXIT_SUCCESS);
            return 0;
        }}
    };

    RawMode _raw; // enable raw mode for single-key reads
    if(!_raw.ok){
        cerr << "Failed to set raw mode\n";
        return 1;
    }

    while (true) {
        int ch = getchar();
        switch (ch) {
            case 'w': case 'W':
                menu.MoveUp();
                break;
            case 's': case 'S':
                menu.MoveDown();
                break;
            case 'e':{
                auto it = actions.find(menu.returnSelected());
                if (it != actions.end()) it->second();
                break;
            }
            case 'q': case 'Q':
                ShowCursor();
                return 0;
            default:
                break;
        }

        ClearScreen();
        Print(_world.timeString());
        _player.printHealth();
        Print("");
        menu.DisplayItems();
        if (_world.isNight()) {

        }
    }
    ShowCursor();
}