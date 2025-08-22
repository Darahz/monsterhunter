#include "../include/World.h"
#include <cmath>
#include <iomanip>
#include <sstream>

World::World() 
    : totalGameSeconds(8.0f * 3600.0f), // Start at 08:00 (8 AM)
      timeScale(60.0f), // 1 real minute = 1 game hour
      newDayFlag(false),
      currentDay(1) {
}

void World::update(float deltaTime) {
    float previousGameSeconds = totalGameSeconds;
    totalGameSeconds += deltaTime * timeScale;
    
    // Check if we've crossed into a new day
    int previousDay = static_cast<int>(previousGameSeconds / (24.0f * 3600.0f));
    int currentDayFromSeconds = static_cast<int>(totalGameSeconds / (24.0f * 3600.0f));
    
    if (currentDayFromSeconds > previousDay) {
        newDayFlag = true;
        updateDayCount();
    } else {
        newDayFlag = false;
    }
}

void World::setTimeScale(float scale) {
    timeScale = std::max(0.1f, scale); // Minimum time scale to prevent issues
}

float World::getTimeScale() const {
    return timeScale;
}

int World::getHours() const {
    // Get seconds within the current day (modulo 24 hours)
    float daySeconds = fmod(totalGameSeconds, 24.0f * 3600.0f);
    return static_cast<int>(daySeconds / 3600.0f);
}

int World::getMinutes() const {
    // Get seconds within the current hour
    float daySeconds = fmod(totalGameSeconds, 24.0f * 3600.0f);
    float hourSeconds = fmod(daySeconds, 3600.0f);
    return static_cast<int>(hourSeconds / 60.0f);
}

int World::getSeconds() const {
    // Get seconds within the current minute
    float daySeconds = fmod(totalGameSeconds, 24.0f * 3600.0f);
    float hourSeconds = fmod(daySeconds, 3600.0f);
    float minuteSeconds = fmod(hourSeconds, 60.0f);
    return static_cast<int>(minuteSeconds);
}

std::string World::getTimeString() const {
    std::ostringstream oss;
    oss << std::setfill('0') << std::setw(2) << getHours() 
        << ":" << std::setw(2) << getMinutes();
    return oss.str();
}

TimeOfDay World::getTimeOfDay() const {
    return calculateTimeOfDay(getHours());
}

std::string World::getTimeOfDayString() const {
    switch (getTimeOfDay()) {
        case TimeOfDay::Dawn:  return "Dawn";
        case TimeOfDay::Day:   return "Day";
        case TimeOfDay::Dusk:  return "Dusk";
        case TimeOfDay::Night: return "Night";
        default:               return "Day";
    }
}

void World::setTime(int hours, int minutes, int seconds) {
    // Clamp values to valid ranges
    hours = std::max(0, std::min(23, hours));
    minutes = std::max(0, std::min(59, minutes));
    seconds = std::max(0, std::min(59, seconds));
    
    // Calculate total seconds for current day
    float daySeconds = hours * 3600.0f + minutes * 60.0f + seconds;
    
    // Preserve the day count by adding the day offset
    float dayOffset = static_cast<float>(currentDay - 1) * 24.0f * 3600.0f;
    totalGameSeconds = dayOffset + daySeconds;
}

bool World::isNewDay() const {
    return newDayFlag;
}

int World::getDayCount() const {
    return currentDay;
}

void World::updateDayCount() {
    currentDay = static_cast<int>(totalGameSeconds / (24.0f * 3600.0f)) + 1;
}

TimeOfDay World::calculateTimeOfDay(int hours) const {
    if (hours >= 5 && hours < 8) {
        return TimeOfDay::Dawn;
    } else if (hours >= 8 && hours < 18) {
        return TimeOfDay::Day;
    } else if (hours >= 18 && hours < 20) {
        return TimeOfDay::Dusk;
    } else {
        return TimeOfDay::Night;
    }
}
