#include "ghost.h"
#include <iostream>
#include <algorithm>
#include <random>

// Ghost类实现
Ghost::Ghost(Position startPos, GhostType ghostType, int speed)
    : position(startPos), previousPosition(startPos), type(ghostType),
      moveSpeed(speed), moveCounter(0), isActive(true),
      currentPatrolIndex(0), patrolForward(true) {

    std::random_device rd;
    gen.seed(rd());

    // 根据鬼类型设置显示字符
    switch(type) {
        case RANDOM_WALKER: displayChar = 'G'; break;    // 普通鬼
        case PATROL_GUARD: displayChar = 'P'; break;     // 巡逻鬼
        case HUNTER: displayChar = 'H'; break;           // 猎人鬼
        case TELEPORTING: displayChar = 'T'; break;      // 传送鬼
        default: displayChar = 'G';
    }
}

void Ghost::update(const Position& playerPos, const std::vector<std::vector<char>>& maze,
                  const std::vector<Position>& chests, const std::vector<Position>& otherGhosts) {

    // 根据移动速度控制更新频率
    moveCounter++;
    if (moveCounter < moveSpeed / 100) { // 简单的速度控制
        return;
    }
    moveCounter = 0;

    previousPosition = position;
    Position newPosition = position;

    // 根据鬼类型选择移动策略
    switch(type) {
        case RANDOM_WALKER:
            newPosition = getRandomMove(maze);
            break;
        case PATROL_GUARD:
            newPosition = getPatrolMove();
            break;
        case HUNTER:
            newPosition = getHunterMove(playerPos, maze);
            break;
        case TELEPORTING:
            newPosition = getTeleportingMove(maze);
            break;
    }

    // 验证移动是否有效（不撞墙、不重叠宝箱、不重叠其他鬼）
    if (isValidMove(newPosition, maze, chests, otherGhosts)) {
        position = newPosition;
    }
}

bool Ghost::checkPlayerCollision(const Position& playerPos) const {
    return position == playerPos;
}

Position Ghost::getRandomMove(const std::vector<std::vector<char>>& maze) {
    std::vector<Position> possibleMoves = getValidAdjacentPositions(position, maze);

    if (possibleMoves.empty()) {
        return position; // 无法移动，保持原位
    }

    std::uniform_int_distribution<> dis(0, possibleMoves.size() - 1);
    return possibleMoves[dis(gen)];
}

Position Ghost::getPatrolMove() {
    if (patrolPath.empty()) {
        return position; // 没有巡逻路径，保持原位
    }

    // 沿巡逻路径移动
    if (patrolForward) {
        currentPatrolIndex++;
        if (currentPatrolIndex >= patrolPath.size()) {
            currentPatrolIndex = patrolPath.size() - 2;
            patrolForward = false;
        }
    } else {
        currentPatrolIndex--;
        if (currentPatrolIndex < 0) {
            currentPatrolIndex = 1;
            patrolForward = true;
        }
    }

    return patrolPath[currentPatrolIndex];
}

Position Ghost::getHunterMove(const Position& playerPos, const std::vector<std::vector<char>>& maze) {
    // 计算到玩家的曼哈顿距离
    int currentDistance = manhattanDistance(position, playerPos);

    // 获取所有可能的移动位置
    std::vector<Position> possibleMoves = getValidAdjacentPositions(position, maze);

    if (possibleMoves.empty()) {
        return position;
    }

    // 寻找能减少与玩家距离的移动方向
    std::vector<Position> bestMoves;
    int bestDistance = currentDistance;

    for (const auto& move : possibleMoves) {
        int newDistance = manhattanDistance(move, playerPos);
        if (newDistance < bestDistance) {
            bestDistance = newDistance;
            bestMoves.clear();
            bestMoves.push_back(move);
        } else if (newDistance == bestDistance) {
            bestMoves.push_back(move);
        }
    }

    // 如果有更好的移动选择，随机选择一个
    if (!bestMoves.empty()) {
        std::uniform_int_distribution<> dis(0, bestMoves.size() - 1);
        return bestMoves[dis(gen)];
    }

    // 没有更好的移动，随机选择
    std::uniform_int_distribution<> dis(0, possibleMoves.size() - 1);
    return possibleMoves[dis(gen)];
}

Position Ghost::getTeleportingMove(const std::vector<std::vector<char>>& maze) {
    // 30%几率传送，70%几率随机移动
    std::uniform_real_distribution<> probDis(0.0, 1.0);

    if (probDis(gen) < 0.3) { // 传送
        std::vector<Position> emptySpaces;
        int height = maze.size();
        int width = maze[0].size();

        // 寻找所有空地位置
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                if (maze[y][x] == ' ') { // 空地
                    emptySpaces.emplace_back(x, y);
                }
            }
        }

        if (!emptySpaces.empty()) {
            std::uniform_int_distribution<> dis(0, emptySpaces.size() - 1);
            return emptySpaces[dis(gen)];
        }
    }

    // 普通随机移动
    return getRandomMove(maze);
}

bool Ghost::isValidMove(const Position& newPos, const std::vector<std::vector<char>>& maze,
                       const std::vector<Position>& chests, const std::vector<Position>& otherGhosts) const {

    // 检查边界
    if (newPos.x < 0 || newPos.x >= maze[0].size() || newPos.y < 0 || newPos.y >= maze.size()) {
        return false;
    }

    // 检查是否是墙
    if (maze[newPos.y][newPos.x] == '#') {
        return false;
    }

    // 检查是否与宝箱重叠
    for (const auto& chest : chests) {
        if (newPos == chest) {
            return false;
        }
    }

    // 检查是否与其他鬼重叠（除了自己之前的位置）
    for (const auto& otherGhost : otherGhosts) {
        if (newPos == otherGhost && newPos != previousPosition) {
            return false;
        }
    }

    return true;
}

int Ghost::manhattanDistance(const Position& a, const Position& b) const {
    return std::abs(a.x - b.x) + std::abs(a.y - b.y);
}

std::vector<Position> Ghost::getValidAdjacentPositions(const Position& current,
                                                      const std::vector<std::vector<char>>& maze) const {
    std::vector<Position> validPositions;
    std::vector<Position> directions = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}}; // 上下左右

    for (const auto& dir : directions) {
        Position newPos(current.x + dir.x, current.y + dir.y);

        // 简单检查：不越界且不是墙
        if (newPos.x >= 0 && newPos.x < maze[0].size() &&
            newPos.y >= 0 && newPos.y < maze.size() &&
            maze[newPos.y][newPos.x] != '#') {
            validPositions.push_back(newPos);
        }
    }

    return validPositions;
}

void Ghost::setPatrolPath(const std::vector<Position>& path) {
    patrolPath = path;
    if (!patrolPath.empty()) {
        position = patrolPath[0];
        currentPatrolIndex = 0;
        patrolForward = true;
    }
}

// GhostManager类实现
GhostManager::GhostManager(int gameDifficulty) : difficulty(gameDifficulty) {
    std::random_device rd;
    gen.seed(rd());
}

void GhostManager::initializeGhosts(int mazeWidth, int mazeHeight, const std::vector<std::vector<char>>& maze) {
    ghosts.clear();
    int ghostCount = getGhostCountForDifficulty();

    for (int i = 0; i < ghostCount; i++) {
        Position ghostPos = findValidGhostPosition(mazeWidth, mazeHeight, maze);
        GhostType type = getGhostTypeForDifficulty(i);
        int speed = 0;

        // 根据难度设置速度
        switch(difficulty) {
            case 1: speed = 500; break; // 简单：慢速
            case 2: speed = 300; break; // 中等：中速
            case 3: speed = 150; break; // 困难：快速
            default: speed = 300;
        }

        Ghost ghost(ghostPos, type, speed);

        // 为巡逻鬼设置巡逻路径
        if (type == PATROL_GUARD) {
            std::vector<Position> patrolPath;
            // 简单的直线巡逻路径示例，实际中可以更复杂
            for (int x = ghostPos.x - 2; x <= ghostPos.x + 2; x += 2) {
                if (x >= 1 && x < mazeWidth - 1 && maze[ghostPos.y][x] != '#') {
                    patrolPath.emplace_back(x, ghostPos.y);
                }
            }
            if (patrolPath.size() > 1) {
                ghost.setPatrolPath(patrolPath);
            }
        }

        ghosts.push_back(ghost);
    }
}

void GhostManager::updateAllGhosts(const Position& playerPos, const std::vector<std::vector<char>>& maze,
                                  const std::vector<Position>& chests) {
    // 收集所有鬼的当前位置（用于重叠检查）
    std::vector<Position> otherGhostsPositions;
    for (const auto& ghost : ghosts) {
        otherGhostsPositions.push_back(ghost.getPosition());
    }

    // 更新每个鬼
    for (auto& ghost : ghosts) {
        ghost.update(playerPos, maze, chests, otherGhostsPositions);
    }
}

bool GhostManager::checkAnyGhostCollision(const Position& playerPos) const {
    for (const auto& ghost : ghosts) {
        if (ghost.checkPlayerCollision(playerPos)) {
            return true;
        }
    }
    return false;
}

void GhostManager::setDifficulty(int newDifficulty) {
    difficulty = newDifficulty;
}

Position GhostManager::findValidGhostPosition(int mazeWidth, int mazeHeight, const std::vector<std::vector<char>>& maze) {
    std::vector<Position> validPositions;

    // 寻找所有有效的鬼位置（空地且远离玩家出生点）
    for (int y = 1; y < mazeHeight - 1; y++) {
        for (int x = 1; x < mazeWidth - 1; x++) {
            if (maze[y][x] == ' ' && // 空地
                (x > 3 || y > 3)) {  // 远离起点(1,1)
                validPositions.emplace_back(x, y);
            }
        }
    }

    if (!validPositions.empty()) {
        std::uniform_int_distribution<> dis(0, validPositions.size() - 1);
        return validPositions[dis(gen)];
    }

    // 如果没有找到合适位置，返回一个默认位置
    return Position(mazeWidth / 2, mazeHeight / 2);
}

GhostType GhostManager::getGhostTypeForDifficulty(int ghostIndex) const {
    switch(difficulty) {
        case 1: // 简单：只有随机移动的鬼
            return RANDOM_WALKER;

        case 2: // 中等：混合类型
            switch(ghostIndex % 3) {
                case 0: return RANDOM_WALKER;
                case 1: return PATROL_GUARD;
                case 2: return HUNTER;
                default: return RANDOM_WALKER;
            }

        case 3: // 困难：更多猎人鬼和传送鬼
            switch(ghostIndex % 4) {
                case 0: return RANDOM_WALKER;
                case 1: return HUNTER;
                case 2: return PATROL_GUARD;
                case 3: return TELEPORTING;
                default: return HUNTER;
            }

        default:
            return RANDOM_WALKER;
    }
}

int GhostManager::getGhostCountForDifficulty() const {
    switch(difficulty) {
        case 1: return 1; // 简单：1个鬼
        case 2: return 3; // 中等：3个鬼
        case 3: return 5; // 困难：5个鬼
        default: return 2;
    }
}