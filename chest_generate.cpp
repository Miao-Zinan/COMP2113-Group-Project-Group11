
#include "chest.h"
#include <vector>
#include <random>
#include <algorithm>
#include <queue>

std::vector<pos> ChestGenerator::generateChests(
    std::vector<std::vector<char>>& maze,
    int startX, int startY,
    int exitX, int exitY,
    int difficulty,
    char chestChar
)
{
    int h = (int)maze.size();
    if (h == 0) return {};
    int w = (int)maze[0].size();

    std::vector<std::vector<bool>> vis(h, std::vector<bool>(w,false));
    std::vector<std::vector<std::pair<int,int>>> parent(
        h, std::vector<std::pair<int,int>>(w, std::make_pair(-1,-1))
    );
    std::queue<std::pair<int,int>> q;
    q.push(std::make_pair(startX, startY));
    vis[startY][startX] = true;
    const int dx[4] = {-1,1,0,0};
    const int dy[4] = {0,0,-1,1};
    bool reached = false;
    while (!q.empty() && !reached) {
        std::pair<int,int> p = q.front(); q.pop();
        int x = p.first, y = p.second;
        for (int i=0;i<4;++i) {
            int nx = x + dx[i], ny = y + dy[i];
            if (nx>=0 && nx<w && ny>=0 && ny<h && !vis[ny][nx] && maze[ny][nx] != '#') {
                vis[ny][nx] = true;
                parent[ny][nx] = std::make_pair(x,y);
                q.push(std::make_pair(nx,ny));
                if (nx == exitX && ny == exitY) { reached = true; break; }
            }
        }
    }

    std::vector<std::vector<bool>> onShortest(h, std::vector<bool>(w,false));
    if (vis[exitY][exitX]) {
        int cx = exitX, cy = exitY;
        while (!(cx == startX && cy == startY)) {
            onShortest[cy][cx] = true;
            std::pair<int,int> pr = parent[cy][cx];
            int px = pr.first, py = pr.second;
            cx = px; cy = py;
            if (cx == -1) break;
        }
        onShortest[startY][startX] = true;
    }

    std::vector<std::vector<bool>> forbidden(h, std::vector<bool>(w,false));
    for (int y=0;y<h;++y) for (int x=0;x<w;++x) {
        if (onShortest[y][x]) forbidden[y][x] = true;
    }
    for (int k=0;k<4;++k) {
        int nx = startX + dx[k], ny = startY + dy[k];
        if (nx>=0 && nx<w && ny>=0 && ny<h) forbidden[ny][nx] = true;
        nx = exitX + dx[k]; ny = exitY + dy[k];
        if (nx>=0 && nx<w && ny>=0 && ny<h) forbidden[ny][nx] = true;
    }
    forbidden[startY][startX] = true;
    forbidden[exitY][exitX] = true;

    std::vector<pos> candidates;
    for (int y=0;y<h;++y) {
        for (int x=0;x<w;++x) {
            if (maze[y][x] == ' ' && !forbidden[y][x]) {
                candidates.push_back(pos{x,y});
            }
        }
    }
    if (candidates.empty()) return {};

    double ratio;
    switch (difficulty) {
        case 1: ratio = 0.03; break;
        case 2: ratio = 0.06; break;
        case 3: ratio = 0.09; break;
        default: ratio = 0.05; break;
    }
    int chestCount = (int)(candidates.size() * ratio);
    if (chestCount < 1) chestCount = 1;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::shuffle(candidates.begin(), candidates.end(), gen);

    std::vector<pos> result;
    for (int i=0;i<chestCount && i < (int)candidates.size(); ++i) {
        pos p = candidates[i];
        result.push_back(p);
        maze[p.y][p.x] = chestChar;
    }
    return result;
}
