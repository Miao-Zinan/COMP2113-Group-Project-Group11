#ifndef PLAYER_H
#define PLAYER_H

class Player {
private:
    int x, y;
    int health;
    int maxHealth;
    bool alive;

public:
    // 构造函数
    Player(int startX = 1, int startY = 1);
    
    // 移动方法 - 位置设置
    void setPosition(int newX, int newY);
    
    // 生命值系统
    void takeDamage();                    // 被鬼攻击后调用
    void increasePlayerHealth();          // 增加生命值
    bool isAlive() const;                 // 检查是否存活
    
    // 获取信息
    int getHealth() const;
    int getX() const;
    int getY() const;
    int getMaxHealth() const;
};

#endif
