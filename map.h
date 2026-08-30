#pragma once

#include <vector>

const int MAP_WIDTH = 8;
const int MAP_HEIGHT = 8;

extern const int map[MAP_HEIGHT][MAP_WIDTH];

void generateMapGeometry(
    std::vector<float>& vertices,
    std::vector<unsigned int>& indices
);
int worldYToMapRow(float y);
bool isWall(float x, float y);