#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H

#include "Player.h"
#include "ghost.h"
#include "maze_generate.h"
#include "chest_generate.h"
#include "chest.h"
#include "fileio.h"
#include "spawnpoint.h"
#include "pos.h"
#include <vector>
#include <atomic>
#include <thread>
#include <chrono>
#include <string>

// Global game state variables (used by chest system)
extern std::atomic<bool> ghostProtection;
extern std::atomic<bool> ghostsStopped;
extern Player* globalPlayer;

// Chest benefit system - needs to access player and ghost manager
void stop_ghosts_temporarily(int seconds);
void enable_ghost_protection_temporarily(int seconds);
void stop_ghost();  // Stops ghosts for 3 seconds
void mingdao();     // Enables ghost protection for 3 seconds

class GameManager {
private:
    // Core game components
    MazeGenerator mazeGen;
    Player* player;
    GhostManager* ghostManager;
    std::vector<pos> chests;
    
    // Game state
    bool isPaused;
    bool gameOver;
    bool gameWon;
    int difficulty;
    int moves;
    
    // Ghost control
    std::thread ghostStopThread;
    
public:
    GameManager();
    ~GameManager();
    
    // Game initialization
    void initializeGame(int difficultyLevel);
    void resetGame();
    
    // Game loop
    void update();
    void handlePlayerMove(int dx, int dy);
    
    // Chest system
    void checkChestCollision();
    void applyChestBenefit();
    void removeChestAt(int x, int y);
    
    // Ghost system
    void updateGhosts();
    void checkGhostCollision();
    
    // Spawnpoint system
    void markSpawnpoint();
    bool goToSpawnpoint();
    bool hasSpawnpoint() const { return spawnpoint_set; }
    
    // Game state
    bool isGamePaused() const { return isPaused; }
    bool isGameOver() const { return gameOver; }
    bool isGameWon() const { return gameWon; }
    void setPaused(bool paused) { isPaused = paused; }
    
    // Save/Load
    bool saveGame(const std::string& filename = "savegame.txt");
    bool loadGame(const std::string& filename = "savegame.txt");
    
    // Getters
    const std::vector<std::vector<char>>& getMaze() const { return mazeGen.getMaze(); }
    Player* getPlayer() const { return player; }
    const std::vector<Ghost>& getGhosts() const { return ghostManager->getGhosts(); }
    const std::vector<pos>& getChests() const { return chests; }
    int getDifficulty() const { return difficulty; }
    int getMoves() const { return moves; }
    int getWidth() const { return mazeGen.getWidth(); }
    int getHeight() const { return mazeGen.getHeight(); }
    int getStartX() const { return mazeGen.getStartX(); }
    int getStartY() const { return mazeGen.getStartY(); }
    int getExitX() const { return mazeGen.getExitX(); }
    int getExitY() const { return mazeGen.getExitY(); }
    
    // Spawnpoint getters
    int getSpawnpointX() const;
    int getSpawnpointY() const;
    
    // Check if position is valid
    bool isValidPosition(int x, int y) const;
    bool isWall(int x, int y) const;
    
private:
    void generateChests();
    void convertChestPositions();
    Position posToPosition(const pos& p) const;
    pos positionToPos(const Position& p) const;
};

#endif // GAMEMANAGER_H

