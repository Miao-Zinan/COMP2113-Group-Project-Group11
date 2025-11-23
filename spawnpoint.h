// spawnpoint.h
#ifndef SPAWNPOINT_H
#define SPAWNPOINT_H

#include "pos.h"

// Spawnpoint position (global variable for compatibility)
extern pos spawnpoint_pos;
extern bool spawnpoint_set;

// Mark the current position as the spawnpoint
// Takes player's current position (x, y)
void mark_spawnpoint(int x, int y);

// Move the player to the spawnpoint position
// Returns true if spawnpoint was set and player was moved, false otherwise
bool go_to_spawnpoint(int& playerX, int& playerY);

#endif

