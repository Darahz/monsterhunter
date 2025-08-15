#include <vector>
#include <iostream>
#include <string>
#include <termios.h>
#include <unistd.h>
#include <unordered_map>
#include <functional>
#include <cctype>
#include <algorithm>

#include <thread>
#include <chrono>

#include "../include/Player.h"
#include "../include/Utils.h"
#include "../include/Recipes.h"
#include "../include/Menu.h"
#include "../include/World.h"


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
                sleep(2);
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
    Menu exploreMenu({"Look around"});
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
                Print("Press any key…"); getchar();
            }else{
                PrintDot(3,250);
                Print("You see a big lake in front of you. The birds are singing a beautiful song");
                PrintDot(3,250);
            }
            PrintDot(3);
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
        bool yn = PrintYesNo("You don't have an axe. Are you sure you want to continue?");
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
    sleep(3);
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
        "Fight",
        "Explore",
        "Chop wood",
        "Rest",
        "Inventory",
        "Craft",
        "Exit"});
    Print(_world.timeString());
    Print("");
    menu.DisplayItems();

    
    unordered_map<string, function<void()>> actions{
        {"Fight", [&]{

        }},{"Explore", [&]{
            OpenExplorationMenu(_player, _world);
        }},
        {"Chop wood", [&]{
            ChopWood(_player);
        }},
        {"Rest", [&]{
            if(PrintYesNo("This will forward the time to 08:00 next day, do you want to continue?")){
                _player.rest(_world);
            }
        }},
        {"Inventory", [&]{
            OpenInventoryMenu(_player);
        }},
        {"Craft", [&]{
            OpenCraftingMenu(_player);
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
                return 0;
            default:
                break;
        }

        ClearScreen();
        Print(_world.timeString());
        Print("");
        menu.DisplayItems();
        if (_world.isNight()) {

        }
    }
    ShowCursor();
}