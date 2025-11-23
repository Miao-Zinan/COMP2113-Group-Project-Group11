#ifndef MAZE_H
#define MAZE_H

#include <vector>
#include <random>

class MazeGenerator {
public:
    MazeGenerator();

    // 1/2/3
    void setDifficulty(int level);

    void generate();

    const std::vector<std::vector<char>>& getMaze() const;
    int getWidth() const;
    int getHeight() const;
    int getStartX() const;
    int getStartY() const;
    int getExitX() const;
    int getExitY() const;
    int getDifficulty() const;

private:
    int width, height;
    int difficulty;
    int startX, startY;
    int exitX, exitY;
    std::vector<std::vector<char>> maze;

    std::random_device rd;
    std::mt19937 gen;

    const int dx[4] = {-1, 1, 0, 0};
    const int dy[4] = {0, 0, -1, 1};

    void dfs(int x, int y, std::vector<std::vector<bool>>& visited);
    void ensureReachable();
    void addExtraPassages();
    void removeOpenAreas();
};

#endif 
