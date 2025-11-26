// spawnpoint.cpp
#include <iostream> 
#include "pos.h"
#include "spawnpoint.h"
using namespace std; 

// Global spawnpoint position
pos spawnpoint_pos = {0, 0};
bool spawnpoint_set = false;

// This function will get the current position and give the value to spawnpoint position. 
void mark_spawnpoint(int x, int y) {
    spawnpoint_pos.x = x;
    spawnpoint_pos.y = y;
    spawnpoint_set = true;
    cout << "Spawnpoint marked at: (" << x << ", " << y << ")" << endl;
}

// This function will get the spawnpoint position and give the value to current position. 
bool go_to_spawnpoint(int& playerX, int& playerY) {
    if (!spawnpoint_set) {
        cout << "Spawnpoint not set!" << endl;
        return false;
    }
    
    playerX = spawnpoint_pos.x;
    playerY = spawnpoint_pos.y;
    cout << "Teleported to spawnpoint: (" << playerX << ", " << playerY << ")" << endl;
    return true;
}

