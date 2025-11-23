#include "fileio.h"
#include <cstdio> // std::rename, std::remove
#include <fstream>
#include <iostream>
#include <sstream>

static std::string tempFilenameFor(const std::string &filename) {
  return filename + ".tmp";
}

bool saveGameToFile(const std::string &filename, const GameState &state,
                    std::string &err) {
  if (state.width <= 0 || state.height <= 0) {
    err = "Invalid dimensions in GameState";
    return false;
  }
  if ((int)state.maze_lines.size() != state.height) {
    err = "Maze line count does not match height";
    return false;
  }
  for (const auto &line : state.maze_lines) {
    if ((int)line.size() != state.width) {
      err = "One or more maze lines do not match width";
      return false;
    }
  }

  std::string tmp = tempFilenameFor(filename);
  std::ofstream ofs(tmp, std::ios::out | std::ios::trunc);
  if (!ofs.is_open()) {
    err = "Failed to open temporary save file for writing: " + tmp;
    return false;
  }

  // Write header same as original format for compatibility:
  // difficulty
  // width height
  // playerX playerY
  // exitX exitY
  // moves
  // then the maze lines
  ofs << state.difficulty << "\n";
  ofs << state.width << " " << state.height << "\n";
  ofs << state.playerX << " " << state.playerY << "\n";
  ofs << state.exitX << " " << state.exitY << "\n";
  ofs << state.moves << "\n";

  for (const auto &line : state.maze_lines) {
    ofs << line << "\n";
  }

  if (!ofs.good()) {
    ofs.close();
    std::remove(tmp.c_str());
    err = "Error while writing to temporary save file";
    return false;
  }
  ofs.close();

  // Attempt atomic replace
  if (std::rename(tmp.c_str(), filename.c_str()) != 0) {
    // rename failed; try to remove tmp and report error
    std::remove(tmp.c_str());
    err = "Failed to rename temporary save file to final filename";
    return false;
  }

  return true;
}

bool loadGameFromFile(const std::string &filename, GameState &state,
                      std::string &err) {
  std::ifstream ifs(filename);
  if (!ifs.is_open()) {
    err = "Save file not found: " + filename;
    return false;
  }

  // Read header values robustly
  int difficulty;
  int width, height;
  int playerX, playerY;
  int exitX, exitY;
  int moves;

  if (!(ifs >> difficulty)) {
    err = "Failed to read difficulty";
    return false;
  }
  if (!(ifs >> width >> height)) {
    err = "Failed to read width and height";
    return false;
  }
  if (!(ifs >> playerX >> playerY)) {
    err = "Failed to read player coordinates";
    return false;
  }
  if (!(ifs >> exitX >> exitY)) {
    err = "Failed to read exit coordinates";
    return false;
  }
  if (!(ifs >> moves)) {
    err = "Failed to read moves";
    return false;
  }

  // consume the rest of the line after moves to position for getline
  std::string line;
  std::getline(ifs, line);

  std::vector<std::string> maze_lines;
  maze_lines.reserve(height);
  for (int y = 0; y < height; ++y) {
    if (!std::getline(ifs, line)) {
      err = "Not enough maze lines in save file";
      return false;
    }
    // If the line contains Windows CR, strip it
    if (!line.empty() && line.back() == '\r')
      line.pop_back();

    if ((int)line.size() != width) {
      std::ostringstream oss;
      oss << "Maze line " << y << " length (" << line.size() << ") != width ("
          << width << ")";
      err = oss.str();
      return false;
    }
    maze_lines.push_back(line);
  }

  // Basic coordinate validation
  if (playerX < 0 || playerX >= width || playerY < 0 || playerY >= height) {
    err = "Player coordinates out of bounds in save file";
    return false;
  }
  if (exitX < 0 || exitX >= width || exitY < 0 || exitY >= height) {
    err = "Exit coordinates out of bounds in save file";
    return false;
  }

  // All good: populate state
  state.difficulty = difficulty;
  state.width = width;
  state.height = height;
  state.playerX = playerX;
  state.playerY = playerY;
  state.exitX = exitX;
  state.exitY = exitY;
  state.moves = moves;
  state.maze_lines = std::move(maze_lines);

  return true;
}