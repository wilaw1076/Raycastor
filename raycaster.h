#pragma once

#include "player.h"
#include "map.h"

struct RayHit
{
    bool hit = false;
    float distance = 0.0f;
    float hitX = 0.0f;
    float hitY = 0.0f;
};

RayHit castRay(const Player& player, float rayAngle);
RayHit castRayDDA(const Player& player, float rayAngle);
void generateRayGeometry(const Player& player, const RayHit& rayHit, std::vector<float>& vertices);