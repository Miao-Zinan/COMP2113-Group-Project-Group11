// mark_spawnpoint.cpp
#include <iostream> 
#include "pos.h"
#include "mark_spawnpoint.h"
using namespace std; 

// This function will get the current position and give the value to spawnpoint position. 
void mark_spawnpoint() {
    // Here current_pos is the current position of the player. It is a pos variable. 
    // spawnpoint_pos is the spawnpoint position set before. It is also a pos variable.
    spawnpoint_pos = current_pos; 
}