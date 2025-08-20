#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <random>
#include <thread>
#include <mutex>
#include <atomic>
#include <chrono>

enum class WeatherType {
    None,
    Snow
};

struct Snowflake {
    sf::Vector2f position;
    sf::Vector2f velocity;
    float windOffset;
    sf::CircleShape shape;
    sf::Uint8 alpha;
};

class Weather {
public:
    Weather();
    ~Weather();

    void setWeatherType(WeatherType type);
    void update(float deltaTime);
    void draw(sf::RenderWindow& window);
    void setWindowSize(float width, float height);
    void startUpdateThread();
    void stopUpdateThread();

private:
    WeatherType currentWeather;
    std::vector<Snowflake> snowflakes;
    float windowWidth;
    float windowHeight;
    float topGap;
    float bottomGap;
    float snowSpawnTimer;
    float snowSpawnRate;
    
    std::mt19937 rng;
    std::uniform_real_distribution<float> xPositionDist;
    std::uniform_real_distribution<float> velocityDist;
    std::uniform_real_distribution<float> windDist;

    std::thread updateThread;
    std::mutex snowflakesMutex;
    std::atomic<bool> threadRunning;
    std::atomic<bool> shouldStop;
    std::chrono::steady_clock::time_point lastUpdateTime;

    void updateSnow(float deltaTime);
    void spawnSnowflake();
    void removeOffscreenSnowflakes();
    void updateSnowflakeFade(Snowflake& snowflake);
    void threadUpdateLoop();
};
