#include "maze_generate.h"
#include <algorithm>
#include <queue>
#include <utility>

MazeGenerator::MazeGenerator()
    : width(0), height(0), difficulty(1), gen(rd())
{
    setDifficulty(1);
}

void MazeGenerator::setDifficulty(int level) {
    difficulty = level;
    switch (level) {
        case 1: width = 31; height = 21; break;
        case 2: width = 51; height = 31; break;
        case 3: width = 71; height = 41; break;
        default: width = 31; height = 21; difficulty = 1; break;
    }
    if (width % 2 == 0) --width;
    if (height % 2 == 0) --height;
    if (width < 3) width = 3;
    if (height < 3) height = 3;
}

void MazeGenerator::generate() {
    maze.assign(height, std::vector<char>(width, '#'));
    std::vector<std::vector<bool>> visited(height, std::vector<bool>(width, false));

    startX = 1; startY = 1;
    exitX = width - 2; exitY = height - 2;
    dfs(startX, startY, visited);
    maze[startY][startX] = ' ';
    maze[exitY][exitX] = ' ';
    ensureReachable();
    addExtraPassages();
    removeOpenAreas();
    maze[startY][startX] = ' ';
    maze[exitY][exitX] = ' ';
}

const std::vector<std::vector<char>>& MazeGenerator::getMaze() const { return maze; }
int MazeGenerator::getWidth() const { return width; }
int MazeGenerator::getHeight() const { return height; }
int MazeGenerator::getStartX() const { return startX; }
int MazeGenerator::getStartY() const { return startY; }
int MazeGenerator::getExitX() const { return exitX; }
int MazeGenerator::getExitY() const { return exitY; }
int MazeGenerator::getDifficulty() const { return difficulty; }

void MazeGenerator::setMaze(const std::vector<std::vector<char>>& loadedMaze, int w, int h,
                            int sx, int sy, int ex, int ey, int diff) {
    maze = loadedMaze;
    width = w;
    height = h;
    startX = sx;
    startY = sy;
    exitX = ex;
    exitY = ey;
    difficulty = diff;
}

void MazeGenerator::dfs(int x, int y, std::vector<std::vector<bool>>& visited) {
    visited[y][x] = true;
    maze[y][x] = ' ';
    std::vector<int> dirs = {0, 1, 2, 3};
    std::shuffle(dirs.begin(), dirs.end(), gen);

    for (int dir : dirs) {
        int nx = x + dx[dir] * 2;
        int ny = y + dy[dir] * 2;
        if (nx > 0 && nx < width - 1 && ny > 0 && ny < height - 1 && !visited[ny][nx]) {
            maze[y + dy[dir]][x + dx[dir]] = ' ';
            dfs(nx, ny, visited);
        }
    }
}

void MazeGenerator::ensureReachable() {
    int h = height, w = width;
    std::vector<std::vector<bool>> vis(h, std::vector<bool>(w, false));
    std::queue<std::pair<int, int>> q;
    q.push(std::make_pair(startX, startY));
    vis[startY][startX] = true;
    while (!q.empty()) {
        std::pair<int, int> p = q.front(); q.pop();
        int x = p.first, y = p.second;
        for (int i = 0; i < 4; ++i) {
            int nx = x + dx[i], ny = y + dy[i];
            if (nx >= 0 && nx < w && ny >= 0 && ny < h && !vis[ny][nx] && maze[ny][nx] != '#') {
                vis[ny][nx] = true;
                q.push(std::make_pair(nx, ny));
            }
        }
    }
    if (vis[exitY][exitX]) return;
    int x = exitX, y = exitY;
    maze[y][x] = ' ';
    while (x != startX || y != startY) {
        if (x > startX) --x;
        else if (x < startX) ++x;
        else if (y > startY) --y;
        else if (y < startY) ++y;
        maze[y][x] = ' ';
    }
}

void MazeGenerator::addExtraPassages() {
    int attempts;
    switch (difficulty) {
        case 1: attempts = width * height / 80; break;
        case 2: attempts = width * height / 50; break;
        case 3: attempts = width * height / 30; break;
        default: attempts = width * height / 50; break;
    }
    if (attempts <= 0) return;

    std::uniform_int_distribution<> xr(1, width - 2);
    std::uniform_int_distribution<> yr(1, height - 2);

    for (int i = 0; i < attempts; ++i) {
        int x = xr(gen), y = yr(gen);
        if (maze[y][x] != '#') continue;

        int pathNeighbors = 0;
        for (int k = 0; k < 4; ++k) {
            int nx = x + dx[k], ny = y + dy[k];
            if (nx >= 0 && nx < width && ny >= 0 && ny < height && maze[ny][nx] == ' ')
                ++pathNeighbors;
        }
        if (pathNeighbors < 2) continue;
        bool forms2x2 = false;
        for (int yy = y - 1; yy <= y; ++yy) {
            for (int xx = x - 1; xx <= x; ++xx) {
                if (xx >= 0 && xx + 1 < width && yy >= 0 && yy + 1 < height) {
                    int empty = 0;
                    for (int ry = 0; ry < 2; ++ry) for (int rx = 0; rx < 2; ++rx) {
                        int cx = xx + rx, cy = yy + ry;
                        if ((cx == x && cy == y) || maze[cy][cx] == ' ') ++empty;
                    }
                    if (empty == 4) { forms2x2 = true; break; }
                }
            }
            if (forms2x2) break;
        }
        if (forms2x2) continue;
        maze[y][x] = ' ';
    }
}

void MazeGenerator::removeOpenAreas() {
    bool changed = true;
    std::uniform_int_distribution<> pickDir(0, 3);
    (void)pickDir; 

    while (changed) {
        changed = false;
        for (int y = 0; y < height - 1; ++y) {
            for (int x = 0; x < width - 1; ++x) {
                if (maze[y][x] == ' ' && maze[y][x + 1] == ' ' &&
                    maze[y + 1][x] == ' ' && maze[y + 1][x + 1] == ' ') {
                    std::vector<std::pair<int, int>> cells = {{x, y}, {x + 1, y}, {x, y + 1}, {x + 1, y + 1}};
                    std::shuffle(cells.begin(), cells.end(), gen);
                    for (auto &c : cells) {
                        int cx = c.first, cy = c.second;
                        if ((cx == startX && cy == startY) || (cx == exitX && cy == exitY)) continue;
                        int openNeighbors = 0;
                        for (int k = 0; k < 4; ++k) {
                            int nx = cx + dx[k], ny = cy + dy[k];
                            if (nx >= 0 && nx < width && ny >= 0 && ny < height && maze[ny][nx] == ' ')
                                ++openNeighbors;
                        }
                        maze[cy][cx] = '#';
                        changed = true;
                        break;
                    }
                }
            }
        }
    }
}
