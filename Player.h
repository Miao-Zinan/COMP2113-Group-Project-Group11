#ifndef PLAYER_H
#define PLAYER_H

class Player {
private:
    int x, y;
    int health;
    int maxHealth;
    bool alive;

public:
    // Constructor
    Player(int startX = 1, int startY = 1);
    
    // Movement method - position setting
    void setPosition(int newX, int newY);
    
    // Health system
    void takeDamage();                    // Called after being attacked by ghost
    void increasePlayerHealth();          // Increase health
    bool isAlive() const;                 // Check if alive
    
    // Get information
    int getHealth() const;
    int getX() const;
    int getY() const;
    int getMaxHealth() const;
};

#endif
