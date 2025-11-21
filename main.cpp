#include <iostream>
#include <vector>
#include <random>
#include <fstream>
#include <string>
#include <thread>
#include <chrono>
#include <unistd.h>
#include <termios.h>
#include <sys/ioctl.h>

using namespace std;

class TerminalController {
public:
    void clearScreen() {
        system("clear");
    }

    void setCursorPosition(int x, int y) {
        printf("\033[%d;%dH", y + 1, x + 1);
    }

    void hideCursor() {
        printf("\033[?25l");
    }

    void showCursor() {
        printf("\033[?25h");
    }

    char getInput() {
        struct termios oldt, newt;
        char ch;
        tcgetattr(STDIN_FILENO, &oldt);
        newt = oldt;
        newt.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);
        ch = getchar();
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
        return ch;
    }
};

class MazeGame {
private:
    vector<vector<char>> maze;
    int playerX, playerY;
    int exitX, exitY;
    int width, height;
    int difficulty;
    int moves;
    TerminalController terminal;

    random_device rd;
    mt19937 gen;

    const int dx[4] = {-1, 1, 0, 0};
    const int dy[4] = {0, 0, -1, 1};

public:
    MazeGame() : gen(rd()) {
        width = 0;
        height = 0;
        difficulty = 1;
        moves = 0;
    }

    void setDifficulty(int level) {
        difficulty = level;
        switch (level) {
            case 1: width = 15; height = 10; break;
            case 2: width = 25; height = 15; break;
            case 3: width = 35; height = 20; break;
            default: width = 15; height = 10;
        }
    }

    void generateMaze() {
        maze.clear();
        maze.resize(height, vector<char>(width, '#'));

        vector<vector<bool>> visited(height, vector<bool>(width, false));

        uniform_int_distribution<> disX(1, width - 2);
        uniform_int_distribution<> disY(1, height - 2);

        int startX = disX(gen);
        int startY = disY(gen);

        dfsGeneration(startX, startY, visited);

        playerX = 1;
        playerY = 1;
        maze[playerY][playerX] = 'P';

        exitX = width - 2;
        exitY = height - 2;
        maze[exitY][exitX] = 'E';

        ensureConnectivity();
        moves = 0;
    }

    void dfsGeneration(int x, int y, vector<vector<bool>>& visited) {
        visited[y][x] = true;
        maze[y][x] = ' ';

        vector<int> directions = {0, 1, 2, 3};
        shuffle(directions.begin(), directions.end(), gen);

        for (int dir : directions) {
            int nx = x + dx[dir] * 2;
            int ny = y + dy[dir] * 2;

            if (nx > 0 && nx < width - 1 && ny > 0 && ny < height - 1 && !visited[ny][nx]) {
                maze[y + dy[dir]][x + dx[dir]] = ' ';
                dfsGeneration(nx, ny, visited);
            }
        }
    }

    void ensureConnectivity() {
        for (int i = 0; i < 4; i++) {
            int nx = playerX + dx[i];
            int ny = playerY + dy[i];
            if (nx >= 0 && nx < width && ny >= 0 && ny < height) {
                maze[ny][nx] = ' ';
            }
        }

        for (int i = 0; i < 4; i++) {
            int nx = exitX + dx[i];
            int ny = exitY + dy[i];
            if (nx >= 0 && nx < width && ny >= 0 && ny < height) {
                maze[ny][nx] = ' ';
            }
        }
    }

    void display() {
        terminal.clearScreen();

        cout << "========================================\n";
        cout << "           迷宫探险游戏\n";
        cout << "========================================\n";
        cout << "难度: ";
        switch (difficulty) {
            case 1: cout << "简单"; break;
            case 2: cout << "中等"; break;
            case 3: cout << "困难"; break;
        }
        cout << " | 移动步数: " << moves << "\n\n";

        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                char cell = maze[y][x];
                switch (cell) {
                    case '#': cout << "██"; break;
                    case ' ': cout << "  "; break;
                    case 'P': cout << "😊"; break;
                    case 'E': cout << "🏁"; break;
                    default: cout << cell << cell;
                }
            }
            cout << "\n";
        }

        cout << "\n控制说明:\n";
        cout << "W/A/S/D - 移动  R - 重新开始  B - 保存  L - 加载  Q - 退出\n";
        cout << "========================================\n";
    }

    bool movePlayer(char direction) {
        int newX = playerX, newY = playerY;

        switch (direction) {
            case 'w': case 'W': newY--; break;
            case 's': case 'S': newY++; break;
            case 'a': case 'A': newX--; break;
            case 'd': case 'D': newX++; break;
            default: return false;
        }

        if (newX < 0 || newX >= width || newY < 0 || newY >= height ||
            maze[newY][newX] == '#') {
            return false;
        }

        maze[playerY][playerX] = ' ';
        playerX = newX;
        playerY = newY;

        if (playerX == exitX && playerY == exitY) {
            maze[playerY][playerX] = 'P';
            moves++;
            return true;
        }

        maze[playerY][playerX] = 'P';
        moves++;
        return false;
    }

    void saveGame() {
        ofstream file("maze_save.txt");
        if (file.is_open()) {
            file << difficulty << "\n";
            file << width << " " << height << "\n";
            file << playerX << " " << playerY << "\n";
            file << exitX << " " << exitY << "\n";
            file << moves << "\n";

            for (int y = 0; y < height; y++) {
                for (int x = 0; x < width; x++) {
                    file << maze[y][x];
                }
                file << "\n";
            }

            file.close();
            cout << "游戏已保存!\n";
        } else {
            cout << "保存失败!\n";
        }
        this_thread::sleep_for(chrono::seconds(1));
    }

    bool loadGame() {
        ifstream file("maze_save.txt");
        if (file.is_open()) {
            file >> difficulty;
            file >> width >> height;
            file >> playerX >> playerY;
            file >> exitX >> exitY;
            file >> moves;

            maze.resize(height, vector<char>(width));
            string line;
            getline(file, line);

            for (int y = 0; y < height; y++) {
                getline(file, line);
                for (int x = 0; x < width; x++) {
                    maze[y][x] = line[x];
                }
            }

            file.close();
            cout << "游戏已加载!\n";
            this_thread::sleep_for(chrono::seconds(1));
            return true;
        } else {
            cout << "没有找到保存文件!\n";
            this_thread::sleep_for(chrono::seconds(1));
            return false;
        }
    }

    int showMenu() {
        terminal.clearScreen();

        cout << "========================================\n";
        cout << "           迷宫探险游戏\n";
        cout << "========================================\n";
        cout << "          欢迎来到迷宫世界!\n\n";
        cout << "请选择难度:\n";
        cout << "1. 简单 (15x10)\n";
        cout << "2. 中等 (25x15)\n";
        cout << "3. 困难 (35x20)\n";
        cout << "4. 加载游戏\n";
        cout << "Q. 退出游戏\n";
        cout << "========================================\n";
        cout << "选择: ";

        char choice;
        cin >> choice;

        switch (choice) {
            case '1': return 1;
            case '2': return 2;
            case '3': return 3;
            case '4': return 4;
            case 'q': case 'Q': return 0;
            default: return 1;
        }
    }

    void run() {
        terminal.hideCursor();

        while (true) {
            int choice = showMenu();

            if (choice == 0) {
                break;
            } else if (choice == 4) {
                if (!loadGame()) {
                    continue;
                }
            } else {
                setDifficulty(choice);
                generateMaze();
            }

            bool gameOver = false;

            while (!gameOver) {
                display();

                char input = terminal.getInput();

                switch (input) {
                    case 'q': case 'Q':
                        return;
                    case 'r': case 'R':
                        generateMaze();
                        break;
                    case 'b': case 'B':  // 修改为B键保存
                        saveGame();
                        break;
                    case 'l': case 'L':
                        if (loadGame()) {
                            display();
                        }
                        break;
                    default:
                        gameOver = movePlayer(input);
                        break;
                }

                if (gameOver) {
                    display();
                    cout << "\n🎉 恭喜! 你成功到达终点!\n";
                    cout << "总移动步数: " << moves << "\n";
                    cout << "按任意键继续...";
                    terminal.getInput();
                    break;
                }
            }
        }

        terminal.showCursor();
        cout << "感谢游玩迷宫探险游戏!\n";
    }
};

int main() {
    MazeGame game;
    game.run();

    return 0;
}
