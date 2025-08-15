#pragma once
#include <iostream>
#include <string>
#include <algorithm>
#include <thread>
#include <chrono>

using namespace std;

enum class Color {
    Default,
    Red,
    Green,
    Yellow,
    Blue,
    Magenta,
    Cyan,
    White
};

void SetColor(Color c);

std::string toLower(const std::string &s);

std::string toFirstUpper(const std::string &s);

void Print(const std::string &str, Color c = Color::Default);

void PrintDot(int sleeper, int sleepTime = 150, Color c = Color::Default);

bool PrintYesNo(string message);

int getRandomNum(int min, int max);

void ClearScreen();

void HideCursor();

void ShowCursor();