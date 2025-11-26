#include "Player.h"
#include <iostream>
#include <thread>
#include <chrono>

using namespace std;

// Constructor: initialize position and health
Player::Player(int startX, int startY) 
    : x(startX), y(startY), health(3), maxHealth(3), alive(true) {}

// Set player position
void Player::setPosition(int newX, int newY) {
    x = newX;
    y = newY;
}

// Take damage: health decreases by one
void Player::takeDamage() {
    if (alive && health > 0) {
        health--;
        cout << "Player attacked! Health: " << health << "/" << maxHealth << endl;
        
        if (health <= 0) {
            alive = false;
            cout << "Game over! Player health is zero!" << endl;
        }
    }
}

// Increase health: maximum is three points
void Player::increasePlayerHealth() {
    if (alive && health < maxHealth) {
        health++;
        cout << "Health increased! Current health: " << health << "/" << maxHealth << endl;
    } else if (health >= maxHealth) {
        cout << "Health is full!" << endl;
    }
}

// Check if player is alive
bool Player::isAlive() const {
    return alive;
}

// Get current health
int Player::getHealth() const {
    return health;
}

// Get player X coordinate
int Player::getX() const {
    return x;
}

// Get player Y coordinate
int Player::getY() const {
    return y;
}

// Get maximum health
int Player::getMaxHealth() const {
    return maxHealth;
}