#include "GameManager.h"
#include "chest.h"
#include "spawnpoint.h"
#include <algorithm>
#include <random>
#include <iostream>
#include <fstream>

// Global variables for chest system
std::atomic<bool> ghostProtection(false);
std::atomic<bool> ghostsStopped(false);
Player* globalPlayer = nullptr;

void stop_ghosts_temporarily(int seconds) {
    ghostsStopped = true;
    std::thread([seconds]() {
        std::this_thread::sleep_for(std::chrono::seconds(seconds));
        ghostsStopped = false;
    }).detach();
}

void enable_ghost_protection_temporarily(int seconds) {
    ghostProtection = true;
    std::thread([seconds]() {
        std::this_thread::sleep_for(std::chrono::seconds(seconds));
        ghostProtection = false;
    }).detach();
}

// These functions are called from chest.cpp
void stop_ghost() {
    stop_ghosts_temporarily(3);
}

void mingdao() {
    enable_ghost_protection_temporarily(3);
}

GameManager::GameManager() 
    : player(nullptr), ghostManager(nullptr), isPaused(false), 
      gameOver(false), gameWon(false), difficulty(1), moves(0) {
    globalPlayer = nullptr;
}

GameManager::~GameManager() {
    delete player;
    delete ghostManager;
    globalPlayer = nullptr;
}

void GameManager::initializeGame(int difficultyLevel) {
    difficulty = difficultyLevel;
    moves = 0;
    isPaused = false;
    gameOver = false;
    gameWon = false;
    
    // Generate maze
    mazeGen.setDifficulty(difficulty);
    mazeGen.generate();
    
    // Create player at start position
    if (player) delete player;
    player = new Player(mazeGen.getStartX(), mazeGen.getStartY());
    globalPlayer = player;
    
    // Generate chests
    generateChests();
    
    // Initialize ghosts
    if (ghostManager) delete ghostManager;
    ghostManager = new GhostManager(difficulty);
    std::vector<std::vector<char>> maze = mazeGen.getMaze();
    ghostManager->initializeGhosts(mazeGen.getWidth(), mazeGen.getHeight(), maze);
    
    // Reset global states
    ghostProtection = false;
    ghostsStopped = false;
    
    // Reset spawnpoint - set initial spawnpoint at start position
    spawnpoint_set = false;
    mark_spawnpoint(mazeGen.getStartX(), mazeGen.getStartY());
}

void GameManager::resetGame() {
    initializeGame(difficulty);
}

void GameManager::generateChests() {
    std::vector<std::vector<char>> maze = mazeGen.getMaze();
    
    // Use chest generator
    chests = ChestGenerator::generateChests(
        maze, 
        mazeGen.getStartX(), mazeGen.getStartY(),
        mazeGen.getExitX(), mazeGen.getExitY(),
        difficulty,
        'C'
    );
}

void GameManager::update() {
    if (isPaused || gameOver || gameWon) return;
    
    // Update ghosts if not stopped
    if (!ghostsStopped) {
        updateGhosts();
    }
    
    // Check collisions
    checkChestCollision();
    checkGhostCollision();
    
    // Check win condition
    if (player->getX() == mazeGen.getExitX() && player->getY() == mazeGen.getExitY()) {
        gameWon = true;
        gameOver = true;
    }
}

void GameManager::handlePlayerMove(int dx, int dy) {
    if (isPaused || gameOver || gameWon) return;
    
    int newX = player->getX() + dx;
    int newY = player->getY() + dy;
    
    if (isValidPosition(newX, newY) && !isWall(newX, newY)) {
        player->setPosition(newX, newY);
        moves++;
        update();
    }
}

bool GameManager::isValidPosition(int x, int y) const {
    return x >= 0 && x < mazeGen.getWidth() && 
           y >= 0 && y < mazeGen.getHeight();
}

bool GameManager::isWall(int x, int y) const {
    if (!isValidPosition(x, y)) return true;
    const auto& maze = mazeGen.getMaze();
    return maze[y][x] == '#';
}

void GameManager::checkChestCollision() {
    int px = player->getX();
    int py = player->getY();
    
    for (auto it = chests.begin(); it != chests.end(); ++it) {
        if (it->x == px && it->y == py) {
            // Player met a chest
            applyChestBenefit();
            chests.erase(it);
            break;
        }
    }
}

void GameManager::applyChestBenefit() {
    if (!player) return;
    
    std::random_device rd;
    std::mt19937 gen(rd());
    int effect = gen() % 3 + 1;
    
    switch (effect) {
        case 1:
            player->increasePlayerHealth();
            std::cout << "Benefit: Health increased!" << std::endl;
            break;
        case 2:
            stop_ghost();
            std::cout << "Benefit: Ghosts stopped for 3 seconds!" << std::endl;
            break;
        case 3:
            mingdao();
            std::cout << "Benefit: Ghost protection for 3 seconds!" << std::endl;
            break;
    }
}

void GameManager::removeChestAt(int x, int y) {
    chests.erase(
        std::remove_if(chests.begin(), chests.end(),
            [x, y](const pos& p) { return p.x == x && p.y == y; }),
        chests.end()
    );
}

void GameManager::updateGhosts() {
    if (!ghostManager) return;
    if (ghostsStopped) return;  // Don't update ghosts if they're stopped
    
    Position playerPos(player->getX(), player->getY());
    std::vector<Position> chestPositions;
    for (const auto& chest : chests) {
        chestPositions.push_back(Position(chest.x, chest.y));
    }
    
    ghostManager->updateAllGhosts(playerPos, mazeGen.getMaze(), chestPositions);
}

void GameManager::checkGhostCollision() {
    if (!player || !player->isAlive()) return;
    if (ghostProtection) return; // Player is protected
    
    Position playerPos(player->getX(), player->getY());
    if (ghostManager->checkAnyGhostCollision(playerPos)) {
        player->takeDamage();
        if (!player->isAlive()) {
            gameOver = true;
        }
    }
}

Position GameManager::posToPosition(const pos& p) const {
    return Position(p.x, p.y);
}

pos GameManager::positionToPos(const Position& p) const {
    return pos{p.x, p.y};
}

void GameManager::markSpawnpoint() {
    if (player) {
        mark_spawnpoint(player->getX(), player->getY());
    }
}

bool GameManager::goToSpawnpoint() {
    if (!player) return false;
    
    int newX = player->getX();
    int newY = player->getY();
    
    if (go_to_spawnpoint(newX, newY)) {
        // Check if the spawnpoint position is valid (not a wall)
        if (isValidPosition(newX, newY) && !isWall(newX, newY)) {
            player->setPosition(newX, newY);
            return true;
        } else {
            std::cout << "Warning: Spawnpoint position is invalid (may be a wall)!" << std::endl;
            return false;
        }
    }
    return false;
}

int GameManager::getSpawnpointX() const {
    if (!hasSpawnpoint()) return 0;
    extern pos spawnpoint_pos;
    return spawnpoint_pos.x;
}

int GameManager::getSpawnpointY() const {
    if (!hasSpawnpoint()) return 0;
    extern pos spawnpoint_pos;
    return spawnpoint_pos.y;
}

bool GameManager::saveGame(const std::string& filename) {
    GameState state;
    state.difficulty = difficulty;
    state.width = mazeGen.getWidth();
    state.height = mazeGen.getHeight();
    state.playerX = player->getX();
    state.playerY = player->getY();
    state.exitX = mazeGen.getExitX();
    state.exitY = mazeGen.getExitY();
    state.moves = moves;
    
    // Convert maze to string lines
    const auto& maze = mazeGen.getMaze();
    state.maze_lines.clear();
    for (const auto& row : maze) {
        std::string line;
        for (char c : row) {
            line += c;
        }
        state.maze_lines.push_back(line);
    }
    
    // Save chest positions in maze
    for (const auto& chest : chests) {
        if (chest.y >= 0 && chest.y < state.height && 
            chest.x >= 0 && chest.x < state.width) {
            state.maze_lines[chest.y][chest.x] = 'C';
        }
    }
    
    std::string err;
    bool success = saveGameToFile(filename, state, err);
    if (!success) {
        std::cerr << "Save error: " << err << std::endl;
    }
    return success;
}

bool GameManager::loadGame(const std::string& filename) {
    GameState state;
    std::string err;
    
    if (!loadGameFromFile(filename, state, err)) {
        std::cerr << "Load error: " << err << std::endl;
        return false;
    }
    
    difficulty = state.difficulty;
    moves = state.moves;
    
    // Reconstruct maze
    std::vector<std::vector<char>> maze;
    chests.clear();
    
    for (int y = 0; y < state.height; y++) {
        std::vector<char> row;
        for (int x = 0; x < state.width; x++) {
            char c = state.maze_lines[y][x];
            if (c == 'C') {
                chests.push_back(pos{x, y});
                row.push_back(' ');
            } else {
                row.push_back(c);
            }
        }
        maze.push_back(row);
    }
    
    // Set up maze generator with loaded maze
    // Note: Start position is where player was saved, exit is at exitX/exitY
    mazeGen.setMaze(maze, state.width, state.height, 
                    state.playerX, state.playerY, state.exitX, state.exitY, difficulty);
    
    // Create player
    if (player) delete player;
    player = new Player(state.playerX, state.playerY);
    globalPlayer = player;
    
    // Initialize ghosts
    if (ghostManager) delete ghostManager;
    ghostManager = new GhostManager(difficulty);
    ghostManager->initializeGhosts(state.width, state.height, maze);
    
    isPaused = false;
    gameOver = false;
    gameWon = false;
    
    return true;
}

