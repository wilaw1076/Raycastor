#include "player.h"
#include "map.h"

constexpr float PI = 3.14159265f;

bool canMoveTo(float x, float y, float radius)
{
    return !isWall(x - radius, y - radius) &&
           !isWall(x + radius, y - radius) &&
           !isWall(x - radius, y + radius) &&
           !isWall(x + radius, y + radius);
}

void movePlayerForward(Player& player, float deltaTime)
{
    float speed = 2.0f;

    float newX = player.x + std::cos(player.angle) * speed * deltaTime;
    float newY = player.y + std::sin(player.angle) * speed * deltaTime;
    
    if(canMoveTo(newX, player.y, player.radius))
    {
        player.x = newX;
    }

    if(canMoveTo(player.x, newY, player.radius))
    {
        player.y = newY;
    }
}

void movePlayerBackward(Player& player, float deltaTime)
{
    float speed = 2.0f;

    float newX = player.x - std::cos(player.angle) * speed * deltaTime;
    float newY = player.y - std::sin(player.angle) * speed * deltaTime;

    if(canMoveTo(newX, player.y, player.radius))
    {
        player.x = newX;
    }

    if(canMoveTo(player.x, newY, player.radius))
    {
        player.y = newY;
    }
}

void rotatePlayerLeft(Player& player, float deltaTime)
{
    float rotationSpeed = 2.0f;

    player.angle += rotationSpeed * deltaTime;
}

void rotatePlayerRight(Player& player, float deltaTime)
{
    float rotationSpeed = 2.0f;

    player.angle -= rotationSpeed * deltaTime;
}

void generatePlayerGeometry(const Player& player, std::vector<float>& vertices, int segments)
{
    float centerX = -1.0f + (player.x / MAP_WIDTH) * 2.0f;
    float centerY = -1.0f + (player.y / MAP_HEIGHT) * 2.0f;

    float radiusX = (player.radius / MAP_WIDTH) * 2.0f;
    float radiusY = (player.radius / MAP_HEIGHT) * 2.0f;

    vertices.push_back(centerX);
    vertices.push_back(centerY);
    vertices.push_back(0.0f);
    
    for (int i = 0; i <= segments; i++)
    {
        float angle = 2.0f * PI * i / segments;

        float x = centerX + std::cos(angle) * radiusX;
        float y = centerY + std::sin(angle) * radiusY;

        vertices.push_back(x);
        vertices.push_back(y);
        vertices.push_back(0.0f);
    }
}

void generateDirectionGeometry(const Player& player, std::vector<float>& vertices)
{
    float lineLength = 0.5f;

    float centerX = -1.0f + (player.x / MAP_WIDTH) * 2.0f;
    float centerY = -1.0f + (player.y / MAP_HEIGHT) * 2.0f;

    float endWorldX = player.x + std::cos(player.angle) * lineLength;
    float endWorldY = player.y + std::sin(player.angle) * lineLength;

    float endX = -1.0f + (endWorldX / MAP_WIDTH) * 2.0f;
    float endY = -1.0f + (endWorldY / MAP_HEIGHT) * 2.0f;

    vertices.push_back(centerX);
    vertices.push_back(centerY);
    vertices.push_back(0.0f);

    vertices.push_back(endX);
    vertices.push_back(endY);
    vertices.push_back(0.0f);
}