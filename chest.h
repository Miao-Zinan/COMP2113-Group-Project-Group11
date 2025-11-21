// chest.h
#ifndef CHEST_H
#define CHEST_H

#include <vector>
#include "pos.h"

std::vector<pos> generate_chests(std::vector<std::vector<char>>& maze, int difficulty, const std::vector<pos>& wallPositions);
bool meet_chest(pos pos_player, std::vector<pos> pos_chests);
std::vector<pos> clear_chest(pos pos_player, std::vector<pos> pos_chests);
void benefit();

#endif

// After each movement of the player, the chest system should:
// 1. check whether whether the player meet a chest, using "meet_chest"
// if the player meet a chest, the chest system should:
// 2. clear the met chest to ensure one chest can only be met by once, using "clear_chest"
// 3. give the player one benefit, using "benefit"