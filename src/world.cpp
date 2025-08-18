#include "World.h"
#include <iomanip>
#include <sstream>
#include <condition_variable>
#include <chrono>

World::World() {}
World::~World() { stop(); }

void World::start() {
    if (running_.exchange(true)) return;            // already running
    worker_ = std::thread(&World::runLoop_, this);
}

void World::stop() {
    if (!running_.exchange(false)) return;          // already stopped
    if (worker_.joinable()) worker_.join();
}

void World::setMillisPerMinute(int ms) {
    if (ms < 1) ms = 1;
    msPerMinute_.store(ms, std::memory_order_relaxed);
}

int World::getHour() const   { std::lock_guard<std::mutex> lk(mtx_); return hour_; }
int World::getMinute() const { std::lock_guard<std::mutex> lk(mtx_); return minute_; }
int World::getDay() const    { std::lock_guard<std::mutex> lk(mtx_); return day_; }

std::string World::timeString() const {
    std::lock_guard<std::mutex> lk(mtx_);
    std::ostringstream os;
    os << "Day " << day_ << ' '
       << std::setw(2) << std::setfill('0') << hour_ << ':'
       << std::setw(2) << std::setfill('0') << minute_;
    return os.str();
}

bool World::isNight() const {
    std::lock_guard<std::mutex> lk(mtx_);
    return (hour_ >= 20 || hour_ < 6);
}

void World::runLoop_() {
    using clock = std::chrono::steady_clock;
    auto next = clock::now();
    while (running_.load(std::memory_order_relaxed)) {
        // wait until next tick based on current speed
        next += std::chrono::milliseconds(msPerMinute_.load(std::memory_order_relaxed));
        std::this_thread::sleep_until(next);
        {
            std::lock_guard<std::mutex> lk(mtx_);
            minute_ += 2;
            if (minute_ >= 60) { minute_ = 0; hour_ += 1; }
            if (hour_   >= 24) { hour_ = 0; day_  += 1; }
        }
    }
}

void World::addHour(int hour){
    this->hour_ += hour;
}

void World::setDay(int day){
    this->day_ = day;
}

void World::setTime(int hour, int minute){
    this->hour_ = hour;
    this->minute_ = minute;
}