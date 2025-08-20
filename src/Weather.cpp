#include "../include/Weather.h"
#include <algorithm>
#include <cmath>

Weather::Weather() : currentWeather(WeatherType::None), windowWidth(800), windowHeight(600),
                     topGap(80.0f), bottomGap(50.0f),
                     snowSpawnTimer(0.0f), snowSpawnRate(0.1f), rng(std::random_device{}()),
                     xPositionDist(0.0f, 800.0f), velocityDist(50.0f, 150.0f), windDist(-30.0f, 30.0f),
                     threadRunning(false), shouldStop(false) {
    lastUpdateTime = std::chrono::steady_clock::now();
}

Weather::~Weather() {
    stopUpdateThread();
}

void Weather::setWeatherType(WeatherType type) {
    std::lock_guard<std::mutex> lock(snowflakesMutex);
    currentWeather = type;
    if (type != WeatherType::Snow) {
        snowflakes.clear();
    }
}

void Weather::setWindowSize(float width, float height) {
    std::lock_guard<std::mutex> lock(snowflakesMutex);
    windowWidth = width;
    windowHeight = height;
    xPositionDist = std::uniform_real_distribution<float>(0.0f, width);
}

void Weather::update(float deltaTime) {
    if (!threadRunning) {
        switch (currentWeather) {
            case WeatherType::Snow:
                updateSnow(deltaTime);
                break;
            case WeatherType::None:
            default:
                break;
        }
    }
}

void Weather::updateSnow(float deltaTime) {
    snowSpawnTimer += deltaTime;
    
    if (snowSpawnTimer >= snowSpawnRate) {
        spawnSnowflake();
        snowSpawnTimer = 0.0f;
    }

    for (auto& snowflake : snowflakes) {
        snowflake.windOffset += deltaTime * 2.0f;
        float windEffect = std::sin(snowflake.windOffset) * 20.0f;
        
        snowflake.position.x += (snowflake.velocity.x + windEffect) * deltaTime;
        snowflake.position.y += snowflake.velocity.y * deltaTime;
        
        snowflake.shape.setPosition(snowflake.position);
        
        updateSnowflakeFade(snowflake);
    }

    removeOffscreenSnowflakes();
}

void Weather::spawnSnowflake() {
    Snowflake snowflake;
    snowflake.position.x = xPositionDist(rng);
    snowflake.position.y = -topGap - 10.0f;
    snowflake.velocity.x = windDist(rng);
    snowflake.velocity.y = velocityDist(rng);
    snowflake.windOffset = 2.0f;
    snowflake.alpha = 255;
    
    snowflake.shape.setRadius(2.0f);
    snowflake.shape.setFillColor(sf::Color::White);
    snowflake.shape.setPosition(snowflake.position);
    
    snowflakes.push_back(snowflake);
}

void Weather::removeOffscreenSnowflakes() {
    snowflakes.erase(
        std::remove_if(snowflakes.begin(), snowflakes.end(),
            [this](const Snowflake& snowflake) {
                return snowflake.position.y > windowHeight + bottomGap + 10.0f ||
                       snowflake.position.x < -10.0f ||
                       snowflake.position.x > windowWidth + 10.0f;
            }),
        snowflakes.end()
    );
}

void Weather::draw(sf::RenderWindow& window) {
    std::lock_guard<std::mutex> lock(snowflakesMutex);
    if (currentWeather == WeatherType::Snow) {
        for (const auto& snowflake : snowflakes) {
            window.draw(snowflake.shape);
        }
    }
}

void Weather::updateSnowflakeFade(Snowflake& snowflake) {
    const float fadeZone = 100.0f;
    float alpha = 255.0f;
    
    float distanceFromLeft = snowflake.position.x;
    float distanceFromRight = windowWidth - snowflake.position.x;
    float distanceFromTop = snowflake.position.y - topGap;
    float distanceFromBottom = (windowHeight - bottomGap) - snowflake.position.y;
    
    float minDistance = std::min({distanceFromLeft, distanceFromRight, distanceFromTop, distanceFromBottom});
    
    if (minDistance < fadeZone) {
        alpha = (minDistance / fadeZone) * 255.0f;
        alpha = std::max(0.0f, alpha);
    }
    
    snowflake.alpha = static_cast<sf::Uint8>(alpha);
    sf::Color color = snowflake.shape.getFillColor();
    color.a = snowflake.alpha;
    snowflake.shape.setFillColor(color);
}

void Weather::startUpdateThread() {
    if (!threadRunning) {
        shouldStop = false;
        threadRunning = true;
        lastUpdateTime = std::chrono::steady_clock::now();
        updateThread = std::thread(&Weather::threadUpdateLoop, this);
    }
}

void Weather::stopUpdateThread() {
    if (threadRunning) {
        shouldStop = true;
        if (updateThread.joinable()) {
            updateThread.join();
        }
        threadRunning = false;
    }
}

void Weather::threadUpdateLoop() {
    const float targetFPS = 60.0f;
    const std::chrono::milliseconds frameDuration(static_cast<int>(1000.0f / targetFPS));
    
    while (!shouldStop) {
        auto currentTime = std::chrono::steady_clock::now();
        auto deltaTimeMs = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastUpdateTime);
        float deltaTime = deltaTimeMs.count() / 1000.0f;
        
        if (deltaTime > 0.0f) {
            std::lock_guard<std::mutex> lock(snowflakesMutex);
            switch (currentWeather) {
                case WeatherType::Snow:
                    updateSnow(deltaTime);
                    break;
                case WeatherType::None:
                default:
                    break;
            }
            lastUpdateTime = currentTime;
        }
        
        std::this_thread::sleep_for(frameDuration);
    }
}
