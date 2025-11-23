#include "Player.h"
#include <iostream>
#include <thread>
#include <chrono>

using namespace std;

// 构造函数：初始化位置和生命值
Player::Player(int startX, int startY) 
    : x(startX), y(startY), health(3), maxHealth(3), alive(true) {}

// 设置玩家位置
void Player::setPosition(int newX, int newY) {
    x = newX;
    y = newY;
}

// 受到伤害：生命值减一
void Player::takeDamage() {
    if (alive && health > 0) {
        health--;
        cout << "玩家受到攻击！生命值: " << health << "/" << maxHealth << endl;
        
        if (health <= 0) {
            alive = false;
            cout << "游戏结束！玩家生命值归零！" << endl;
        }
    }
}

// 增加生命值：上限为三点
void Player::increasePlayerHealth() {
    if (alive && health < maxHealth) {
        health++;
        cout << "生命值增加！当前生命值: " << health << "/" << maxHealth << endl;
    } else if (health >= maxHealth) {
        cout << "生命值已满！" << endl;
    }
}

// 检查玩家是否存活
bool Player::isAlive() const {
    return alive;
}

// 获取当前生命值
int Player::getHealth() const {
    return health;
}

// 获取玩家X坐标
int Player::getX() const {
    return x;
}

// 获取玩家Y坐标
int Player::getY() const {
    return y;
}

// 获取最大生命值
int Player::getMaxHealth() const {
    return maxHealth;
}
