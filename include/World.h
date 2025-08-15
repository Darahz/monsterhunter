#pragma once
#include <thread>
#include <atomic>
#include <mutex>
#include <string>
#include <chrono>

class World {
public:
    World();
    ~World();                 // guarantees the thread stops

    void start();             // launch the time thread (no-op if already running)
    void stop();              // stop the thread

    void setMillisPerMinute(int ms); // how fast 1 in-game minute passes
    int  getHour()  const;
    int  getMinute()const;
    int  getDay()   const;

    void addHour(int hour);
    void setDay(int day);
    void setTime(int hour, int minute);

    std::string timeString() const;  // e.g. "Day 3 14:07"
    bool isNight() const;            // convenience: night hours

private:
    void runLoop_();                 // the worker thread

    // state
    mutable std::mutex mtx_;
    int hour_   = 8;                 // start morning-ish
    int minute_ = 0;
    int day_    = 1;

    // timing
    std::atomic<int> msPerMinute_{250}; // 1 game minute = 250ms by default
    std::atomic<bool> running_{false};
    std::thread worker_;
};
