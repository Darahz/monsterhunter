#pragma once

#include <string>

class Player;

class Injury {
public:
    Injury(const std::string& type, int severity, int duration)
        : type(type), severity(severity), duration(duration) {};
    std::string getType() const;
    int getSeverity() const;
    int getDuration() const;
    void update();
    void destroy();

private:
    std::string type;
    int severity;
    int duration;
    Player* player;
};