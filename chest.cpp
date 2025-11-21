#include <iostream>
#include <vector>
#include <random>
#include <cstdlib>
#include <thread>
#include <chrono>
#include <atomic>
#include "chest.h"
#include "pos.h"
// #include "map.h" //调用 map - to be implemented
// #include "ghost.h" //调用ghost - to be implemented
// #include "player.h" //调用player - to be implemented
using namespace std;

vector<pos> generate_chests(vector<vector<char>>& maze, int difficulty, const vector<pos>& wallPositions){
    vector<pos> chests;
    int height = maze.size();
    int width = maze[0].size(); // The area of the map

    int chestCount = 0;
    switch (difficulty){
        case 1: chestCount = 4; break;
        case 2: chestCount = 3; break;
        case 3: chestCount = 2; break;
    } // different difficulties have different number of chests

    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> disX(1, width - 2);
    uniform_int_distribution<> disY(1, height - 2);

    while (chests.size() < chestCount){
        int x = disX(gen);
        int y = disY(gen);

        if (maze[y][x] == ' '){
            bool isWall = false;
            for (const pos& wall : wallPositions){
                if (wall.x == x && wall.y == y){
                    isWall = true;
                    break;
                } // check whether the pos is on the wall
            }
            
            // Check if a chest already exists at this position
            bool isDuplicate = false;
            for (const pos& chest : chests){
                if (chest.x == x && chest.y == y){
                    isDuplicate = true;
                    break;
                }
            }
            
            if (!isWall && !isDuplicate){ //generate a chest only when it isn't on the wall and not a duplicate
                maze[y][x] = 'C'; // 用 'C' 表示宝箱
                chests.push_back(pos{x, y});
            }
        }
    }

    return chests;
} // generate chests, returns the vector that contains all the positions of chests

bool meet_chest(pos pos_player, vector<pos> pos_chests){ // check whether the player meets the chests after any move
    int num = pos_chests.size(); // the number of chests
    for (int i=0;i<num;i++){
        if (pos_player.x == pos_chests[i].x && pos_player.y == pos_chests[i].y){
            return true;
        }
    }
    return false;
} // returns true if the player meet the chest

vector<pos> clear_chest(pos pos_player, vector<pos> pos_chests){ // pos_player is just the pos of the met chest
    // call this function after meet_chest returns true such that one met chest should be removed
    vector<pos> result;
    int num = pos_chests.size();
    for (int i=0;i<num;i++){
        if (!(pos_player.x == pos_chests[i].x && pos_player.y == pos_chests[i].y)){
            result.push_back(pos_chests[i]); // append the pos of the chest only if it is not the met chest
        }
    }
    return result;
}


void mingdao() {
    ghostProtection = true; // open the protection mode
    std::thread( {
        std::this_thread::sleep_for(std::chrono::seconds(3));
        ghostProtection = false; // close the protection mode after three seconds
    }).detach();
}


void benefit(){
    switch (rand() % 3 + 1) { // randomly returns 1/2/3, each one is associated with a kind of benefit
        case 1: increasePlayerHealth(); break; // the first benefit: add one blood to the player
        case 2: stop_ghost(); break; // the second benefit: stop ghost for three seconds
        case 3: mingdao(); break; // the third benefit: can't be hurt by the ghost for three seconds
    }
}