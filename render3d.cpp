#include "render3d.h"

void generate3DWallGeometry(
    const Player& player,
    const std::vector<RayHit>& rayHits,
    std::vector<float>& wallVertices,
    float startAngle,
    float angleStep,
    float fov,
    int viewWidth,
    int viewHeight
)
{
    float projectionPlaneDistance = (viewWidth / 2.0f) /std::tan(fov / 2.0f);

    for (int i = 0; i < static_cast<int>(rayHits.size()); i++)
    {
        float rayAngle = startAngle + i * angleStep;

        RayHit hit = rayHits[i];

        if (!hit.hit)
        {
            continue;
        }

        float correctedDistance = hit.distance * std::cos(rayAngle - player.angle);
        float wallHeightPixels = projectionPlaneDistance / correctedDistance;
        float halfWallHeightNDC = wallHeightPixels / viewHeight;
        float sliceWidth = 2.0f / rayHits.size();

        float topY = halfWallHeightNDC;
        float bottomY = -halfWallHeightNDC;

        if(topY > 1.0f)
        {
            topY = 1.0f;
        }
        if(bottomY < -1.0f)
        {
            bottomY = -1.0f;
        }
        

        float leftX = -1.0f + i * sliceWidth;
        float rightX = leftX + sliceWidth;

        // Triangle 1
        wallVertices.push_back(leftX);
        wallVertices.push_back(topY);
        wallVertices.push_back(0.0f);

        wallVertices.push_back(leftX);
        wallVertices.push_back(bottomY);
        wallVertices.push_back(0.0f);

        wallVertices.push_back(rightX);
        wallVertices.push_back(bottomY);
        wallVertices.push_back(0.0f);

        // Triangle 2
        wallVertices.push_back(leftX);
        wallVertices.push_back(topY);
        wallVertices.push_back(0.0f);

        wallVertices.push_back(rightX);
        wallVertices.push_back(bottomY);
        wallVertices.push_back(0.0f);

        wallVertices.push_back(rightX);
        wallVertices.push_back(topY);
        wallVertices.push_back(0.0f);
    }
}