# Raycastor

A real-time raycasting engine built in C++ and OpenGL. The project delves into DDA-based raycasting, 2D map visualization, and Wolfenstein-style pseudo-3D rendering.

---

Good things to mention for your project:
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

Optional, but this would make your repo stand out.

### Ray Direction

```cpp
rayDirX = cos(rayAngle);
rayDirY = sin(rayAngle);
