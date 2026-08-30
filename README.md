# Raycastor

A real-time raycasting engine built in C++ and OpenGL. The project delves into DDA-based raycasting, 2D map visualization, and Wolfenstein-style pseudo-3D rendering.

---
## Implementations
- DDA raycasting
- player movement and collision
- configurable FOV
- fisheye correction
- top-down 2D ray visualization
- pseudo-3D wall rendering
- split-screen 2D / 3D view

---

## Technologies Used

- C++
- OpenGL
- GLFW
- GLAD
- Make / MinGW
---
## How It Works

1. The player position and angle determine the direction of each ray.
2. Rays are distributed across the field of view.
3. DDA checks which grid cells each ray passes through.
4. When a ray hits a wall, the hit distance is stored.
5. The distance is corrected for fisheye distortion.
6. The corrected distance is converted into a projected wall height.
7. OpenGL draws the resulting wall slices.
---

## Raycasting Math

### DDA (Digital Differential Analyzer)

DDA is a 2D grid-traversal algorithm used to efficiently determine which map cells a ray passes through. Instead of advancing the ray in small fixed increments, DDA jumps directly from one grid boundary to the next.

The initialization stage determines the distance from the player's position to the first vertical and horizontal grid boundaries.

### Ray Direction

Each ray direction is represented by its horizontal and vertical components:

```cpp
rayDirX = cos(rayAngle);
rayDirY = sin(rayAngle);
