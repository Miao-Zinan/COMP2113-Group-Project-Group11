#include "ghost.h"
#include <iostream>
#include <algorithm>
#include <random>

// Ghost class implementation
Ghost::Ghost(Position startPos, GhostType ghostType, int speed)
    : position(startPos), previousPosition(startPos), type(ghostType),
      moveSpeed(speed), moveCounter(0), isActive(true),
      currentPatrolIndex(0), patrolForward(true) {

    std::random_device rd;
    gen.seed(rd());

    // Set display character based on ghost type
    switch(type) {
        case RANDOM_WALKER: displayChar = 'G'; break;    // Normal ghost
        case PATROL_GUARD: displayChar = 'P'; break;     // Patrol ghost
        case HUNTER: displayChar = 'H'; break;           // Hunter ghost
        case TELEPORTING: displayChar = 'T'; break;      // Teleporting ghost
        default: displayChar = 'G';
    }
}

void Ghost::update(const Position& playerPos, const std::vector<std::vector<char>>& maze,
                  const std::vector<Position>& chests, const std::vector<Position>& otherGhosts) {

    // Control update frequency based on move speed
    moveCounter++;
    if (moveCounter < moveSpeed / 100) { // Simple speed control
        return;
    }
    moveCounter = 0;

    previousPosition = position;
    Position newPosition = position;

    // Select movement strategy based on ghost type
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

    // Validate if move is valid (no wall collision, no chest overlap, no other ghost overlap)
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
        return position; // Cannot move, stay in place
    }

    std::uniform_int_distribution<> dis(0, possibleMoves.size() - 1);
    return possibleMoves[dis(gen)];
}

Position Ghost::getPatrolMove() {
    if (patrolPath.empty()) {
        return position; // No patrol path, stay in place
    }

    // Move along patrol path
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
    // Calculate Manhattan distance to player
    int currentDistance = manhattanDistance(position, playerPos);

    // Get all possible move positions
    std::vector<Position> possibleMoves = getValidAdjacentPositions(position, maze);

    if (possibleMoves.empty()) {
        return position;
    }

    // Find move direction that reduces distance to player
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

    // If there are better move options, randomly select one
    if (!bestMoves.empty()) {
        std::uniform_int_distribution<> dis(0, bestMoves.size() - 1);
        return bestMoves[dis(gen)];
    }

    // No better move, randomly select
    std::uniform_int_distribution<> dis(0, possibleMoves.size() - 1);
    return possibleMoves[dis(gen)];
}

Position Ghost::getTeleportingMove(const std::vector<std::vector<char>>& maze) {
    // 30% chance to teleport, 70% chance to move randomly
    std::uniform_real_distribution<> probDis(0.0, 1.0);

    if (probDis(gen) < 0.3) { // Teleport
        std::vector<Position> emptySpaces;
        int height = maze.size();
        int width = maze[0].size();

        // Find all empty space positions
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                if (maze[y][x] == ' ') { // Empty space
                    emptySpaces.emplace_back(x, y);
                }
            }
        }

        if (!emptySpaces.empty()) {
            std::uniform_int_distribution<> dis(0, emptySpaces.size() - 1);
            return emptySpaces[dis(gen)];
        }
    }

    // Normal random move
    return getRandomMove(maze);
}

bool Ghost::isValidMove(const Position& newPos, const std::vector<std::vector<char>>& maze,
                       const std::vector<Position>& chests, const std::vector<Position>& otherGhosts) const {

    // Check boundaries
    if (newPos.x < 0 || newPos.x >= maze[0].size() || newPos.y < 0 || newPos.y >= maze.size()) {
        return false;
    }

    // Check if it's a wall
    if (maze[newPos.y][newPos.x] == '#') {
        return false;
    }

    // Check if overlapping with chest
    for (const auto& chest : chests) {
        if (newPos == chest) {
            return false;
        }
    }

    // Check if overlapping with other ghosts (except own previous position)
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
    std::vector<Position> directions = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}}; // Up, down, left, right

    for (const auto& dir : directions) {
        Position newPos(current.x + dir.x, current.y + dir.y);

        // Simple check: not out of bounds and not a wall
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

// GhostManager class implementation
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

        // Set speed based on difficulty
        switch(difficulty) {
            case 1: speed = 500; break; // Easy: slow
            case 2: speed = 300; break; // Medium: medium speed
            case 3: speed = 150; break; // Hard: fast
            default: speed = 300;
        }

        Ghost ghost(ghostPos, type, speed);

        // Set patrol path for patrol ghost
        if (type == PATROL_GUARD) {
            std::vector<Position> patrolPath;
            // Simple straight-line patrol path example, can be more complex in practice
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
    // Collect current positions of all ghosts (for overlap check)
    std::vector<Position> otherGhostsPositions;
    for (const auto& ghost : ghosts) {
        otherGhostsPositions.push_back(ghost.getPosition());
    }

    // Update each ghost
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

    // Find all valid ghost positions (empty space and far from player spawn point)
    for (int y = 1; y < mazeHeight - 1; y++) {
        for (int x = 1; x < mazeWidth - 1; x++) {
            if (maze[y][x] == ' ' && // Empty space
                (x > 3 || y > 3)) {  // Far from starting point (1,1)
                validPositions.emplace_back(x, y);
            }
        }
    }

    if (!validPositions.empty()) {
        std::uniform_int_distribution<> dis(0, validPositions.size() - 1);
        return validPositions[dis(gen)];
    }

    // If no suitable position found, return a default position
    return Position(mazeWidth / 2, mazeHeight / 2);
}

GhostType GhostManager::getGhostTypeForDifficulty(int ghostIndex) const {
    switch(difficulty) {
        case 1: // Easy: only randomly moving ghosts
            return RANDOM_WALKER;

        case 2: // Medium: mixed types
            switch(ghostIndex % 3) {
                case 0: return RANDOM_WALKER;
                case 1: return PATROL_GUARD;
                case 2: return HUNTER;
                default: return RANDOM_WALKER;
            }

        case 3: // Hard: more hunter and teleporting ghosts
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
        case 1: return 1; // Easy: 1 ghost
        case 2: return 3; // Medium: 3 ghosts
        case 3: return 5; // Hard: 5 ghosts
        default: return 2;
    }
}