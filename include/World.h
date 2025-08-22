#pragma once

#include <SFML/System.hpp>
#include <string>

enum class TimeOfDay {
    Dawn,     // 05:00 - 07:59
    Day,      // 08:00 - 17:59
    Dusk,     // 18:00 - 19:59
    Night     // 20:00 - 04:59
};

class World {
public:
    World();
    
    void update(float deltaTime);
    
    // Time management
    void setTimeScale(float scale); // How fast time passes (1.0 = real time, 60.0 = 1 real minute = 1 game hour)
    float getTimeScale() const;
    
    // Get current time
    int getHours() const;
    int getMinutes() const;
    int getSeconds() const;
    
    // Get formatted time string
    std::string getTimeString() const;
    
    // Get time of day
    TimeOfDay getTimeOfDay() const;
    std::string getTimeOfDayString() const;
    
    // Set time manually (for testing/debugging)
    void setTime(int hours, int minutes, int seconds = 0);
    
    // Check if it's a new day
    bool isNewDay() const;
    
    // Get day count since game start
    int getDayCount() const;

private:
    float totalGameSeconds;    // Total seconds since game start
    float timeScale;          // Time multiplier (how fast time passes)
    bool newDayFlag;          // Flag to indicate a new day has started
    int currentDay;           // Current day count
    
    void updateDayCount();
    TimeOfDay calculateTimeOfDay(int hours) const;
};
