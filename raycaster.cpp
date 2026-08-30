#include <cmath>
#include "raycaster.h"
#include "map.h"

RayHit castRay(const Player& player, float rayAngle)
{
    RayHit rayHit;

    float rayDirX = std::cos(rayAngle);
    float rayDirY = std::sin(rayAngle);

    float rayX = player.x;
    float rayY = player.y;

    float stepSize = 0.01f;
    float maxDistance = 20.0f;

    while (rayHit.distance < maxDistance)
    {
        rayX += rayDirX * stepSize;
        rayY += rayDirY * stepSize;

        rayHit.distance += stepSize;

        if(isWall(rayX, rayY))
        {
            rayHit.hit = true;
            rayHit.hitX = rayX;
            rayHit.hitY = rayY;

            return rayHit;
        }

    }
    return rayHit;
}

RayHit castRayDDA(const Player& player, float rayAngle)
{
    RayHit rayHit;

    float rayDirX = std::cos(rayAngle);
    float rayDirY = std::sin(rayAngle);

    float deltaDistX = std::abs(1.0f / rayDirX);
    float deltaDistY = std::abs(1.0f / rayDirY);    

    int mapX = static_cast<int>(player.x);
    int mapY = static_cast<int>(player.y);

    int stepX, stepY;
    float rayDistX, rayDistY;
    float maxDistance = 20.0f;

    if (rayDirX < 0)
    {
        stepX = -1;
        rayDistX = (player.x - mapX) * deltaDistX;
    }
    else
    {
        stepX = 1;
        rayDistX = (mapX + 1.0f - player.x) * deltaDistX;
    }

    if (rayDirY < 0)
    {
        stepY = -1;
        rayDistY = (player.y - mapY) * deltaDistY;
    }
    else
    {
        stepY = 1;
        rayDistY = (mapY + 1.0f - player.y) * deltaDistY;
    }

    while(!rayHit.hit && rayHit.distance < maxDistance)
    {
        if(rayDistX < rayDistY)
        {
            mapX += stepX;
            rayHit.distance = rayDistX;
            rayDistX += deltaDistX;
        }
        else
        {
            mapY += stepY;
            rayHit.distance = rayDistY;
            rayDistY += deltaDistY;
        }

        if(isWall(mapX + 0.5f, mapY + 0.5f))
        {
            rayHit.hit = true;

            rayHit.hitX = player.x + rayDirX * rayHit.distance;
            rayHit.hitY = player.y + rayDirY * rayHit.distance;
        }
    }
    return rayHit;
}

void generateRayGeometry(const Player& player, const RayHit& rayHit, std::vector<float>& vertices)
{
    float startX = -1.0f + (player.x / MAP_WIDTH) * 2.0f;
    float startY = -1.0f + (player.y / MAP_HEIGHT) * 2.0F;

    float endX = -1.0f + (rayHit.hitX / MAP_WIDTH) * 2.0f;
    float endY = -1.0f + (rayHit.hitY / MAP_HEIGHT) * 2.0f;

    vertices.push_back(startX);
    vertices.push_back(startY);
    vertices.push_back(0.0f);

    vertices.push_back(endX);
    vertices.push_back(endY);
    vertices.push_back(0.0f);
}