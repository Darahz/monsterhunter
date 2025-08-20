#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <random>

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

    void updateSnow(float deltaTime);
    void spawnSnowflake();
    void removeOffscreenSnowflakes();
    void updateSnowflakeFade(Snowflake& snowflake);
};
