#include "../include/Injury.h"

std::string Injury::getType() const {
    return type;
}

int Injury::getSeverity() const {
    return severity;
}

int Injury::getDuration() const {
    return duration;
}

void Injury::update() {
    duration--;
    if(duration > 0){
        this->player->hurt(severity);
    }
    return;
}

void Injury::destroy() {
    // Set everything to default hopefully not to damage the player any further
    this->duration = 0;
    this->severity = 0;
    this->player = nullptr;
}
