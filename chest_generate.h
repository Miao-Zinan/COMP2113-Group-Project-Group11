
#ifndef CHEST_H
#define CHEST_H

#include <vector>
#include "pos.h"

struct pos;

class ChestGenerator {
public:
    static std::vector<pos> generateChests(
        std::vector<std::vector<char>>& maze,
        int startX, int startY,
        int exitX, int exitY,
        int difficulty,
        char chestChar = '$'
    );
};

#endif
