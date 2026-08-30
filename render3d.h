#pragma once

#include <vector>

#include "player.h"
#include "raycaster.h"

void generate3DWallGeometry(
    const Player& player,
    const std::vector<RayHit>& rayHits,
    std::vector<float>& wallVertices,
    float startAngle,
    float angleStep,
    float fov,
    int viewWidth,
    int viewHeight
);