#include "../include/Player.h"

Player::Player() {
    this->timeAlive = 0.0f;
    this->setName("Darahz");
    clock.restart();
}

Player::~Player() {
    std::cout << "Player destructor called for: " << this->getName() << std::endl;
    std::cout << "Time Alive: " << this->timeAlive << " seconds" << std::endl;
}

void Player::update() {
    this->timeAlive = clock.getElapsedTime().asSeconds();

    for (auto& injury : injuries) {
        injury.update();
    }
}

void Player::render() {
    
}
