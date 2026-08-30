#include "map.h"

const int map[MAP_HEIGHT][MAP_WIDTH] =
{
    {1,1,1,1,1,1,1,1},
    {1,0,0,0,0,0,0,1},
    {1,0,1,0,1,0,0,1},
    {1,0,0,0,1,0,0,1},
    {1,0,0,0,0,1,0,1},
    {1,0,1,1,0,0,0,1},
    {1,0,0,0,0,0,0,1},
    {1,1,1,1,1,1,1,1}
};

int worldYToMapRow(float y)
{
    return MAP_HEIGHT - 1 - static_cast<int>(y);
}

void generateMapGeometry(
    std::vector<float>& vertices,
    std::vector<unsigned int>& indices
)
{
    float tileWidth = 2.0f / MAP_WIDTH;
    float tileHeight = 2.0f / MAP_HEIGHT;

    for (int row = 0; row < MAP_HEIGHT; row++)
    {
        for (int col = 0; col < MAP_WIDTH; col++)
        {
            if (map[row][col] == 1)
            {
                float left = -1.0f + col * tileWidth;
                float right = left + tileWidth;

                float top = 1.0f - row * tileHeight;
                float bottom = top - tileHeight;

                unsigned int baseIndex =
                    static_cast<unsigned int>(vertices.size() / 3);

                vertices.insert(vertices.end(), {
                    left,  bottom, 0.0f,
                    right, bottom, 0.0f,
                    right, top,    0.0f,
                    left,  top,    0.0f
                });

                indices.insert(indices.end(), {
                    baseIndex + 0,
                    baseIndex + 1,
                    baseIndex + 2,

                    baseIndex + 2,
                    baseIndex + 3,
                    baseIndex + 0
                });
            }
        }
    }
}

bool isWall(float x, float y)
{
    if(x < 0.0f || x >= MAP_WIDTH || y < 0.0f || y >= MAP_HEIGHT)
    {
        return true;
    }

    int col = static_cast<int>(x);
    int row = worldYToMapRow(y);

    return map[row][col] == 1;
}