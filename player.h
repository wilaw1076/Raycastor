#pragma once

#include <cmath>
#include <vector>

struct Player
{
    float x = 1.5f;
    float y = 1.5f;
    float angle = 0.0f;
    float radius = 0.12f;
};

void movePlayerForward(Player& player, float deltaTime);
void movePlayerBackward(Player& player, float deltaTime);

void rotatePlayerLeft(Player& player, float deltaTime);
void rotatePlayerRight(Player& player, float deltaTime);

void generatePlayerGeometry(const Player& player, std::vector<float>& vertices, int segments);
void generateDirectionGeometry(const Player& Player, std::vector<float>& vertices);

bool canMoveTo(float x, float y, float radius);