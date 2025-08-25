#include <vector>
#include <iostream>
#include <string>
#include <termios.h>
#include <unistd.h>
#include <unordered_map>
#include <functional>
#include <ctime>
#include <sstream>

#include <thread>
#include <chrono>

#include "../include/Player.h"
#include "../include/ItemFactory.h"
#include "../include/Utils.h"
#include "../include/Recipes.h"
#include "../include/Menu.h"
#include "../include/World.h"
#include "../include/SaveHandler.h"

using std::string;
using std::getline;
using std::cin;
using std::ws;
using std::to_string;
using std::stoi;


// Simple item factory for known items


// Developer/debug menu for adding items and other cheats
void OpenDevMenu(Player& player) {
    while (true) {
        ClearScreen();
        Print("== Dev Mode ==");
        Print("1. Add item to inventory");
        Print("2. Heal player");
        Print("3. Set health to 1");
        Print("4. Back");
        Print("");
        Print("Enter option number:");
        int ch = getchar();
        if (ch == '1') {
            // Temporarily restore normal terminal mode for input
            termios oldt; tcgetattr(STDIN_FILENO, &oldt);
            termios newt = oldt; newt.c_lflag |= (ICANON | ECHO);
            tcsetattr(STDIN_FILENO, TCSANOW, &newt);
            string itemName;
            int amount = 1;
            Print("Enter item name:");
            getline(cin >> ws, itemName);
            Print("Enter amount:");
            string amtStr;
            getline(cin >> ws, amtStr);
            try { amount = stoi(amtStr); } catch (...) { amount = 1; }
            tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
            Item item = getItemByName(itemName);
            player.addItem(item, amount);
            Print("Added " + to_string(amount) + "x " + item.name + ". Press any key..."); getchar();
        } else if (ch == '2') {
            player.setHealth(100);
            Print("Player healed to 100 HP. Press any key..."); getchar();
        } else if (ch == '3') {
            player.setHealth(1);
            Print("Player health set to 1. Press any key..."); getchar();
        } else if (ch == '4') {
            return;
        }
    }
}
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
            newt.c_lflag &= ~(ICANON | ECHO);
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
    if (totalWood > 0) {
        _player.addItem(getItemByName("Wood"), totalWood);
        Print(std::string("Managed to get ") + std::to_string(totalWood) + "x wood.");
    }
    if(getRandomNum(1,100) <= 35){
        int apples = totalWood > 0 ? totalWood : 1;
        _player.addItem(getItemByName("Apple"), apples);
        Print(std::string("And ") + std::to_string(apples) + "x Apples.");
    }
    Print("Press any key…"); getchar();
}

void OpenCraftingMenu(Player &_player) {
    // Extract recipe names into a vector
    std::vector<std::pair<std::string, std::function<void()>>> menuItems;
    for (const auto &r : RECIPES) {
        menuItems.push_back({r.first, [&, name = r.first]{
            ClearScreen();
            if (_player.craftItem(name)) {
                std::cout << "Crafted " << name << "!" << std::endl;
            } else {
                std::cout << "Failed to craft " << name << "!" << std::endl;
            }
            Print("Press any key…"); getchar();
        }});
    }
    menuItems.push_back({"Back", [&]{}});
    Menu craftMenu(menuItems);
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
            case 'e':
                if (craftMenu.returnSelected() == "Back") return;
                craftMenu.Select();
                break;
            case 'q': case 'Q':
                return;
        }
    }
}

static std::vector<std::string> BuildInventoryItems(Player& _player) {
    auto list = _player.getInventoryList(); // vector<pair<Item,int>>
    std::vector<std::string> items;
    items.reserve(list.size() + 1);
    for (auto &p : list) {
        const Item& item = p.first;
        int count = p.second;
        std::string desc = item.description.empty() ? "" : (" - " + item.description);
        std::string eq = item.equipable ? " (Equipable)" : "";
        items.push_back(item.name + eq + " x" + std::to_string(count) + desc);
    }
    items.push_back("Back");
    return items;
}

void OpenInventoryMenu(Player &_player) {
    int selectedIndex = 0;
    while (true) {
        auto list = _player.getInventoryList();
        std::vector<std::pair<std::string, std::function<void()>>> menuItems;
        for (const auto& p : list) {
            const Item& item = p.first;
            int count = p.second;
            std::string eq = item.equipable ? " (Equipable)" : "";
            std::string label = item.name + eq + " x" + std::to_string(count);
            menuItems.push_back({label, [&, item]{
                ClearScreen();
                Print(item.name);
                if (!item.description.empty()) {
                    std::cout << "\033[90m" << item.description << "\033[0m" << std::endl;
                }
                Print("");
                std::string actions = "";
                if (item.equipable) actions += "E = Equip, ";
                if (item.edible) actions += "A = Eat, ";
                actions += "D = Drop 1, B = Back";
                Print(actions);
                while (true) {
                    int a = getchar();
                    if ((a == 'E' || a == 'e') && item.equipable) {
                        if (_player.equipItem(item.name)) {
                            Print(std::string("Equipped: ") + item.name, Color::Green);
                        } else {
                            Print("You can't equip that item.", Color::Red);
                        }
                        Print("Press any key…"); getchar();
                        break;
                    } else if ((a == 'A' || a == 'a') && item.edible) {
                        if (_player.getItemCount(item.name) > 0) {
                            _player.removeItem(item.name, 1);
                            _player.AddHealth(15); // Eating restores 15 health
                            Print("You eat the " + item.name + " and feel refreshed!", Color::Green);
                        } else {
                            Print("You don't have that item.", Color::Red);
                        }
                        Print("Press any key…"); getchar();
                        break;
                    } else if (a == 'D' || a == 'd') {
                        if (_player.getItemCount(item.name) > 0) {
                            _player.removeItem(item.name, 1);
                            Print(std::string("Dropped 1 ") + item.name, Color::Yellow);
                        } else {
                            Print("You don't have that item.", Color::Red);
                        }
                        Print("Press any key…"); getchar();
                        break;
                    } else if (a == 'B' || a == 'b' || a == 'q' || a == 'Q') {
                        break;
                    }
                }
            }});
        }
        menuItems.push_back({"Back", [&]{}});
        Menu invMenu(menuItems);
        // Clamp selectedIndex to valid range
        if (selectedIndex >= (int)menuItems.size()) selectedIndex = (int)menuItems.size() - 1;
        if (selectedIndex < 0) selectedIndex = 0;
        // Set the menu's selected index
        while (invMenu.returnSelected() != menuItems[selectedIndex].first) {
            invMenu.MoveDown();
        }
        ClearScreen();
        Print("== Inventory ==");
        if (!_player.getEquipped().empty())
            Print(std::string("Equipped: ") + _player.getEquipped());
        Print("");
        invMenu.DisplayItems();
        int ch = getchar();
        switch (ch) {
            case 'w': case 'W':
                selectedIndex--;
                break;
            case 's': case 'S':
                selectedIndex++;
                break;
            case 'e':
                if (invMenu.returnSelected() == "Back") return;
                invMenu.Select();
                tcflush(STDIN_FILENO, TCIFLUSH);
                break;
            case 'q': case 'Q':
            case 'b': case 'B':
                return;
            default:
                break;
        }
        // Clamp after input
        if (selectedIndex >= (int)menuItems.size()) selectedIndex = 0;
        if (selectedIndex < 0) selectedIndex = (int)menuItems.size() - 1;
    }
}

void OpenExplorationMenu(Player &_player, World &_world){
    Menu exploreMenu({
        {"Fight",    [&]{ /* No action for Fight yet */ }},
        {"Chop wood",[&]{ ChopWood(_player); }},
        {"Look around", [&]{
            int rndNum = getRandomNum(1,5);
            if(rndNum <= 2){
                Print("You look around");
                PrintDot(3);
                Print("You see nothing out of the ordinary. But you did spot a nice shiny stone!");
                _player.addItem(getItemByName("Stone"), getRandomNum(1,2));
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
        }}
    });
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
            case 'e':
                exploreMenu.Select();
                break;
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
    Menu playerMenu({
        {"Inventory", [&]{ OpenInventoryMenu(_player); }},
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
        {"Craft", [&]{ OpenCraftingMenu(_player); }},
        {"Rest", [&]{
            if(PrintYesNo("This will forward the time to 08:00 next day, do you want to continue?")){
                _player.rest(_world);
            }
        }}
    });
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
            case 'e':
                playerMenu.Select();
                break;
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
    std::vector<std::pair<std::string, std::function<void()>>> menuItems;
    for (const auto& file : saveFiles) {
        menuItems.push_back({file, [&, file]{
            if (PrintYesNo("Load save: " + file + "? Current progress will be lost!")) {
                if (SaveHandler::loadGame(_player, _world, file)) {
                    Print("Save loaded successfully!");
                    Print("This is now your active save file.", Color::Cyan);
                    Print("Press any key to continue...");
                    getchar();
                }
            }
        }});
    }
    menuItems.push_back({"Back", [&]{}});
    Menu loadMenu(menuItems);
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
            case 'e':
                if (loadMenu.returnSelected() == "Back") return;
                loadMenu.Select();
                break;
            case 'q': case 'Q':
                return;
        }
    }
}

void SaveHandlingMenu(Player &_player, World &_world){
    _world.stop();
    
    std::vector<std::pair<std::string, std::function<void()>>> menuItems;
    auto makeDefaultSaveName = [&_player]() {
        time_t now = time(0);
        tm* ltm = localtime(&now);
        std::string pname = _player.getName();
        // Remove spaces and non-alphanumeric chars from name for filename safety
        std::string safeName;
        for (char c : pname) {
            if (isalnum(c)) safeName += c;
        }
        if (safeName.empty()) safeName = "Player";
        char timeStr[100];
        sprintf(timeStr, "%s_%04d%02d%02d_%02d%02d%02d",
                safeName.c_str(),
                1900 + ltm->tm_year, 1 + ltm->tm_mon, ltm->tm_mday,
                ltm->tm_hour, ltm->tm_min, ltm->tm_sec);
        return std::string(timeStr);
    };
    if (SaveHandler::hasActiveSave()) {
        menuItems = {
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
                std::string saveName = makeDefaultSaveName();
                Print("Save name: " + saveName);
                if (SaveHandler::saveGame(_player, _world, saveName)) {
                    Print("This is now your active save file.");
                    Print("Press any key to continue...");
                    getchar();
                }
            }},
            {"Load Game", [&]{ OpenLoadMenu(_player, _world); }},
            {"Back", [&]{ _world.start(); }}
        };
    } else {
        menuItems = {
            {"Save Game", [&]{
                ClearScreen();
                Print("Enter save name (or press Enter for default):");
                std::string saveName = makeDefaultSaveName();
                Print("Save name: " + saveName);
                if (SaveHandler::saveGame(_player, _world, saveName)) {
                    Print("This is now your active save file.");
                    Print("Press any key to continue...");
                    getchar();
                }
            }},
            {"Load Game", [&]{ OpenLoadMenu(_player, _world); }},
            {"Back", [&]{ _world.start(); }}
        };
    }
    Menu SaveMenu(menuItems);
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
            case 'e':
                SaveMenu.Select();
                break;
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
    std::string title = "MonsterHunter v.0.0.1";
    std::cout << "\033]0;" << title << "\007";
    HideCursor();
    ClearScreen();

    RawMode _raw; // enable raw mode for single-key reads
    if(!_raw.ok){
        cerr << "Failed to set raw mode\n";
        return 1;
    }

    bool startMenu = true;
    int startMenuIndex = 0;
    while (startMenu) {
        ClearScreen();
        Print("=== MonsterHunter ===\n");
        std::vector<std::pair<std::string, std::function<void()>>> startItems;
        startItems.push_back({"New Game", [&]{
            _player = Player();
            _world.~World();
            new (&_world) World();
            // Temporarily restore normal terminal mode for name input
            termios oldt;
            tcgetattr(STDIN_FILENO, &oldt);
            termios newt = oldt;
            newt.c_lflag |= (ICANON | ECHO);
            tcsetattr(STDIN_FILENO, TCSANOW, &newt);
            ClearScreen();
            Print("Enter your name:");
            std::string pname;
            std::getline(std::cin >> std::ws, pname);
            if (pname.empty()) pname = "Hunter";
            _player.setName(pname);
            // Re-enable raw mode
            tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
            _world.start();
            startMenu = false;
        }});
        if (SaveHandler::hasActiveSave()) {
            startItems.push_back({"Continue", [&]{
                if (SaveHandler::loadGame(_player, _world, SaveHandler::getActiveSave())) {
                    _world.start();
                    startMenu = false;
                }
            }});
        }
        startItems.push_back({"Load Game", [&]{
            OpenLoadMenu(_player, _world);
        }});
        startItems.push_back({"Quit", [&]{
            ShowCursor();
            std::exit(EXIT_SUCCESS);
        }});
        Menu startMenuObj(startItems);
        // Restore selection
        while (startMenuObj.returnSelected() != startItems[startMenuIndex].first) {
            startMenuObj.MoveDown();
        }
        startMenuObj.DisplayItems();
        int ch = getchar();
        switch (ch) {
            case 'w': case 'W':
                startMenuObj.MoveUp();
                break;
            case 's': case 'S':
                startMenuObj.MoveDown();
                break;
            case 'e':
                startMenuObj.Select();
                break;
            case 'q': case 'Q':
                ShowCursor();
                return 0;
            default:
                break;
        }
        // Save selection for next redraw
        for (int i = 0; i < (int)startItems.size(); ++i) {
            if (startMenuObj.returnSelected() == startItems[i].first) {
                startMenuIndex = i;
                break;
            }
        }
    }

    Menu menu({
        {"Explore", [&]{ OpenExplorationMenu(_player, _world); }},
        {"Player",  [&]{ OpenPlayerMenu(_player, _world); }},
        {"Data",    [&]{ SaveHandlingMenu(_player, _world); }},
        {"Exit",    [&]{ ShowCursor(); std::exit(EXIT_SUCCESS); }}
    });
    Print(_world.timeString());
    _player.printHealth();
    Print("");
    menu.DisplayItems();

    while (true) {
        int ch = getchar();
        switch (ch) {
            case 'w': case 'W':
                menu.MoveUp();
                break;
            case 's': case 'S':
                menu.MoveDown();
                break;
            case 'e':
                menu.Select();
                break;
            case 'l': case 'L':
                OpenDevMenu(_player);
                break;
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