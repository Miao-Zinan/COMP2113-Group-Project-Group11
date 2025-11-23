#ifndef GHOST_H
#define GHOST_H

#include <vector>
#include <random>
#include <chrono>
#include <thread>

// 位置结构体，与项目中其他部分保持一致
struct Position {
    int x;
    int y;
    Position(int x = 0, int y = 0) : x(x), y(y) {}
    bool operator==(const Position& other) const {
        return x == other.x && y == other.y;
    }
};

// 鬼的类型枚举
enum GhostType {
    RANDOM_WALKER,    // 随机移动的鬼
    PATROL_GUARD,     // 固定路线巡逻的鬼  
    HUNTER,           // 追逐玩家的鬼
    TELEPORTING       // 会传送的鬼
};

class Ghost {
private:
    Position position;
    Position previousPosition; // 上一次的位置（用于移动痕迹）
    GhostType type;
    int moveSpeed;    // 移动速度（毫秒）
    int moveCounter;  // 移动计数器
    bool isActive;
    char displayChar; // 显示字符
    
    // 移动相关
    std::vector<Position> patrolPath; // 巡逻路径（仅对PATROL_GUARD有效）
    int currentPatrolIndex;
    bool patrolForward; // 巡逻方向
    
    // 随机数生成器
    std::mt19937 gen;
    
public:
    // 构造函数
    Ghost(Position startPos, GhostType ghostType, int speed);
    
    // 核心功能方法
    void update(const Position& playerPos, const std::vector<std::vector<char>>& maze, 
                const std::vector<Position>& chests, const std::vector<Position>& otherGhosts);
    bool checkPlayerCollision(const Position& playerPos) const;
    
    // 获取方法
    Position getPosition() const { return position; }
    GhostType getType() const { return type; }
    char getDisplayChar() const { return displayChar; }
    bool getIsActive() const { return isActive; }
    
    // 设置方法
    void setPosition(Position newPos) { position = newPos; }
    void setPatrolPath(const std::vector<Position>& path);
    
private:
    // 移动策略方法
    Position getRandomMove(const std::vector<std::vector<char>>& maze);
    Position getPatrolMove();
    Position getHunterMove(const Position& playerPos, const std::vector<std::vector<char>>& maze);
    Position getTeleportingMove(const std::vector<std::vector<char>>& maze);
    
    // 辅助方法
    bool isValidMove(const Position& newPos, const std::vector<std::vector<char>>& maze, 
                    const std::vector<Position>& chests, const std::vector<Position>& otherGhosts) const;
    int manhattanDistance(const Position& a, const Position& b) const;
    std::vector<Position> getValidAdjacentPositions(const Position& current, 
                                                   const std::vector<std::vector<char>>& maze) const;
};

class GhostManager {
private:
    std::vector<Ghost> ghosts;
    int difficulty;
    std::mt19937 gen;
    
public:
    GhostManager(int gameDifficulty);
    
    // 鬼群管理方法
    void initializeGhosts(int mazeWidth, int mazeHeight, const std::vector<std::vector<char>>& maze);
    void updateAllGhosts(const Position& playerPos, const std::vector<std::vector<char>>& maze, 
                        const std::vector<Position>& chests);
    bool checkAnyGhostCollision(const Position& playerPos) const;
    
    // 获取方法
    const std::vector<Ghost>& getGhosts() const { return ghosts; }
    int getGhostCount() const { return ghosts.size(); }
    
    // 难度相关方法
    void setDifficulty(int newDifficulty);
    
private:
    Position findValidGhostPosition(int mazeWidth, int mazeHeight, const std::vector<std::vector<char>>& maze);
    GhostType getGhostTypeForDifficulty(int ghostIndex) const;
    int getGhostCountForDifficulty() const;
};

#endif