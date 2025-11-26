#ifndef GHOST_H
#define GHOST_H

#include <vector>
#include <random>
#include <chrono>
#include <thread>

// Position struct, consistent with other parts of the project
struct Position {
    int x;
    int y;
    Position(int x = 0, int y = 0) : x(x), y(y) {}
    bool operator==(const Position& other) const {
        return x == other.x && y == other.y;
    }
    bool operator!=(const Position& other) const {
        return !(*this == other);
    }
};

// Ghost type enumeration
enum GhostType {
    RANDOM_WALKER,    // Random walker ghost
    PATROL_GUARD,     // Patrol guard ghost
    HUNTER,           // Hunter ghost
    TELEPORTING       // Teleporting ghost
};

class Ghost {
private:
    Position position;
    Position previousPosition; // Previous position (for movement trace)
    GhostType type;
    int moveSpeed;    // Move speed (milliseconds)
    int moveCounter;  // Move counter
    bool isActive;
    char displayChar; // Display character
    
    // Movement related
    std::vector<Position> patrolPath; // Patrol path (only valid for PATROL_GUARD)
    int currentPatrolIndex;
    bool patrolForward; // Patrol direction
    
    // Random number generator
    std::mt19937 gen;
    
public:
    // Constructor
    Ghost(Position startPos, GhostType ghostType, int speed);
    
    // Core functionality methods
    void update(const Position& playerPos, const std::vector<std::vector<char>>& maze, 
                const std::vector<Position>& chests, const std::vector<Position>& otherGhosts);
    bool checkPlayerCollision(const Position& playerPos) const;
    
    // Getter methods
    Position getPosition() const { return position; }
    GhostType getType() const { return type; }
    char getDisplayChar() const { return displayChar; }
    bool getIsActive() const { return isActive; }
    
    // Setter methods
    void setPosition(Position newPos) { position = newPos; }
    void setPatrolPath(const std::vector<Position>& path);
    
private:
    // Movement strategy methods
    Position getRandomMove(const std::vector<std::vector<char>>& maze);
    Position getPatrolMove();
    Position getHunterMove(const Position& playerPos, const std::vector<std::vector<char>>& maze);
    Position getTeleportingMove(const std::vector<std::vector<char>>& maze);
    
    // Helper methods
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
    
    // Ghost group management methods
    void initializeGhosts(int mazeWidth, int mazeHeight, const std::vector<std::vector<char>>& maze);
    void updateAllGhosts(const Position& playerPos, const std::vector<std::vector<char>>& maze, 
                        const std::vector<Position>& chests);
    bool checkAnyGhostCollision(const Position& playerPos) const;
    
    // Getter methods
    const std::vector<Ghost>& getGhosts() const { return ghosts; }
    int getGhostCount() const { return ghosts.size(); }
    
    // Difficulty-related methods
    void setDifficulty(int newDifficulty);
    
private:
    Position findValidGhostPosition(int mazeWidth, int mazeHeight, const std::vector<std::vector<char>>& maze);
    GhostType getGhostTypeForDifficulty(int ghostIndex) const;
    int getGhostCountForDifficulty() const;
};

#endif