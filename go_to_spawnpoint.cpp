// go_to_spawnpoint.cpp
#include <iostream> 
#include "pos.h"
#include "go_to_spawnpoint.h"
using namespace std; 

// This function will get the spawnpoint position and give the value to current position. 
void go_to_spawnpoint() {
    // Here current_pos is the current position of the player. It is a pos variable. 
    // spawnpoint_pos is the spawnpoint position set before. It is also a pos variable. 
    current_pos = spawnpoint_pos; 
    
}