#ifndef FILEIO_H
#define FILEIO_H

#include <string>
#include <vector>

struct GameState {
    int difficulty = 1;
    int width = 0;
    int height = 0;
    int playerX = 0;
    int playerY = 0;
    int exitX = 0;
    int exitY = 0;
    int moves = 0;
    // store maze as lines (width characters each)
    std::vector<std::string> maze_lines;
};

// Save the given state to filename. On error, returns false and fills err.
// Saves atomically by writing to filename + ".tmp" then renaming.
bool saveGameToFile(const std::string& filename, const GameState& state, std::string& err);

// Load a saved game from filename into state. On error, returns false and fills err.
bool loadGameFromFile(const std::string& filename, GameState& state, std::string& err);

#endif // FILEIO_H