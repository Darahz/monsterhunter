#include "../include/Utils.h"

void SetColor(Color c) {
    switch (c) {
        case Color::Red:     std::cout << "\x1b[31m"; break;
        case Color::Green:   std::cout << "\x1b[32m"; break;
        case Color::Yellow:  std::cout << "\x1b[33m"; break;
        case Color::Blue:    std::cout << "\x1b[34m"; break;
        case Color::Magenta: std::cout << "\x1b[35m"; break;
        case Color::Cyan:    std::cout << "\x1b[36m"; break;
        case Color::White:   std::cout << "\x1b[37m"; break;
        case Color::Default:
        default:             std::cout << "\x1b[0m";  break; // reset
    }
    std::cout << std::flush;
}

std::string toLower(const std::string &s) {
    std::string out = s;
    std::transform(out.begin(), out.end(), out.begin(),
                   [](unsigned char c){ return std::tolower(c); });
    return out;
}

std::string toFirstUpper(const std::string &s) {
    if (s.empty()) return s; // handle empty string

    std::string out = s;
    // make all lower
    std::transform(out.begin(), out.end(), out.begin(),
                   [](unsigned char c){ return std::tolower(c); });

    // uppercase the first character
    out[0] = static_cast<char>(std::toupper(out[0]));
    return out;
}

void Print(const std::string &str, Color c) {
    SetColor(c);
    std::cout << str << std::endl;
    SetColor(Color::Default); // reset after
}

void PrintDot(int sleeper, int sleepTime, Color c) {
    SetColor(c);
    for (int j = 0; j < sleeper; j++) { 
        std::this_thread::sleep_for(std::chrono::milliseconds(sleepTime));
        std::cout << "." << std::flush;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(sleepTime));
    std::cout << "\n";
    SetColor(Color::Default); // reset
}

bool PrintYesNo(string message){
    Print(message);
    int ch = getchar();
    return ch == 'Y' || ch == 'y';
}


int getRandomNum(int min, int max){
    return rand()%(max-min + 1) + min;
}

int IScrCleanCount = 0;
void ClearScreen(){
    std::cout << "\x1b[2J\x1b[H" << std::flush;
    IScrCleanCount++;
    if (IScrCleanCount % 100 == 0) {
        system("clear");
    }
}

void HideCursor() {
    std::cout << "\x1b[?25l" << std::flush;
}

void ShowCursor() {
    std::cout << "\x1b[?25h" << std::flush;
}
