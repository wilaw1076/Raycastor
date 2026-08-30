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

The raycaster uses the DDA algorithm to efficiently move through the 2D map grid. Instead of moving a ray forward in small increments, DDA determines which vertical or horizontal grid boundary the ray reaches next and moves directly into the corresponding grid cell.

### Ray Direction

Each ray is represented by a unit direction vector calculated from its angle:

```cpp
float rayDirX = cos(rayAngle);
float rayDirY = sin(rayAngle);
```

Using the unit circle,

\[
rayDirX = \cos(\theta)
\]

\[
rayDirY = \sin(\theta)
\]

and therefore:

\[
rayDirX^2 + rayDirY^2 = 1
\]

The player's field of view is divided evenly between all rays:

```cpp
float startAngle = player.angle - FOV / 2.0f;
float angleStep = FOV / (NUM_RAYS - 1);
float rayAngle = startAngle + i * angleStep;
```

`startAngle` represents the left edge of the player's field of view, while `angleStep` determines the angular distance between neighboring rays.

---

### Grid Position

The player's position is stored using floating-point coordinates, but DDA also needs to know which grid cell the ray currently occupies:

```cpp
int mapX = static_cast<int>(player.x);
int mapY = static_cast<int>(player.y);
```

For example, a player at `(2.4, 3.7)` is inside grid cell `(2, 3)`.

---

### Distance Between Grid Boundaries

DDA calculates how far the ray must travel for its X or Y coordinate to change by one full grid unit.

For the X direction:

\[
\Delta x = t \cdot rayDirX
\]

Setting the change in X equal to one grid unit:

\[
1 = t \cdot rayDirX
\]

Solving for \(t\):

\[
t = \frac{1}{rayDirX}
\]

Since distance must be positive:

```cpp
float deltaDistX = abs(1.0f / rayDirX);
float deltaDistY = abs(1.0f / rayDirY);
```

Therefore:

\[
deltaDistX = \left|\frac{1}{rayDirX}\right|
\]

\[
deltaDistY = \left|\frac{1}{rayDirY}\right|
\]

These values represent the distance along the ray between consecutive vertical and horizontal grid crossings.

---

### Ray Step Direction

The sign of the ray direction determines which direction the DDA moves through the grid:

```cpp
if (rayDirX < 0)
    stepX = -1;
else
    stepX = 1;

if (rayDirY < 0)
    stepY = -1;
else
    stepY = 1;
```

A positive X direction moves right, while a negative X direction moves left. A positive Y direction moves upward, while a negative Y direction moves downward.

---

### Distance to the First Grid Boundary

The player may begin anywhere inside a grid cell, so the first grid boundary is usually closer than a full `deltaDist`.

For a ray traveling left:

```cpp
rayDistX = (player.x - mapX) * deltaDistX;
```

For a ray traveling right:

```cpp
rayDistX = (mapX + 1.0f - player.x) * deltaDistX;
```

The same calculation is performed for Y:

```cpp
rayDistY = (player.y - mapY) * deltaDistY;
```

or:

```cpp
rayDistY = (mapY + 1.0f - player.y) * deltaDistY;
```

`rayDistX` and `rayDistY` represent the distance from the player's position to the next vertical and horizontal grid boundaries.

---

### DDA Grid Traversal

During each DDA iteration, the two distances are compared:

```cpp
if (rayDistX < rayDistY)
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
```

If `rayDistX` is smaller, the ray reaches a vertical grid boundary first. Otherwise, it reaches a horizontal grid boundary first.

After entering the next grid cell, the raycaster checks whether that cell contains a wall.

Once a wall is reached, the exact hit position can be calculated using:

\[
position = origin + direction \times distance
\]

```cpp
rayHit.hitX =
    player.x + rayDirX * rayHit.distance;

rayHit.hitY =
    player.y + rayDirY * rayHit.distance;
```

---

## 3D Projection Math

The DDA algorithm provides the distance from the player to the wall. This distance is then converted into the height of a vertical wall slice in the pseudo-3D view.

### Fisheye Correction

Rays near the edges of the field of view travel at an angle and therefore have a longer raw distance than the center ray.

Using these raw distances directly would cause flat walls to appear curved.

The angle between the current ray and the player's forward direction is:

\[
\theta = rayAngle - playerAngle
\]

The forward component of the ray distance is found using cosine:

\[
correctedDistance =
rayDistance \cos(\theta)
\]

```cpp
float correctedDistance =
    hit.distance *
    cos(rayAngle - player.angle);
```

This produces the perpendicular distance to the wall and removes the fisheye distortion.

---

### Projection Plane Distance

An imaginary projection plane is placed in front of the player. Its distance is determined using half of the horizontal field of view and half of the viewport width.

Using:

\[
\tan(\theta) =
\frac{opposite}{adjacent}
\]

gives:

\[
\tan(FOV/2) =
\frac{viewWidth/2}{projectionPlaneDistance}
\]

Therefore:

\[
projectionPlaneDistance =
\frac{viewWidth/2}{\tan(FOV/2)}
\]

```cpp
float projectionPlaneDistance =
    (viewWidth / 2.0f) /
    tan(fov / 2.0f);
```

---

### Projected Wall Height

The apparent wall height is calculated using similar triangles:

\[
\frac{projectedWallHeight}
{projectionPlaneDistance}
=
\frac{actualWallHeight}
{correctedDistance}
\]

Solving for the projected height:

\[
projectedWallHeight =
\frac{
actualWallHeight \cdot projectionPlaneDistance
}{
correctedDistance
}
\]

Because each map wall is treated as one world unit tall:

```cpp
float wallHeightPixels =
    projectionPlaneDistance /
    correctedDistance;
```

This creates the perspective effect:

- closer walls produce larger wall slices
- farther walls produce smaller wall slices

---

### Converting Wall Height to OpenGL Coordinates

OpenGL uses normalized device coordinates ranging from `-1` to `1`.

The projected wall height is converted from pixels into NDC:

```cpp
float halfWallHeightNDC =
    wallHeightPixels / viewHeight;

float topY = halfWallHeightNDC;
float bottomY = -halfWallHeightNDC;
```

Each ray also receives an equal portion of the screen width:

```cpp
float sliceWidth =
    2.0f / rayHits.size();

float leftX =
    -1.0f + i * sliceWidth;

float rightX =
    leftX + sliceWidth;
```

Each ray therefore produces a rectangular vertical wall slice. The collection of these slices forms the final pseudo-3D scene.
### Ray Direction

Each ray direction is represented by its horizontal and vertical components:

```cpp
rayDirX = cos(rayAngle);
rayDirY = sin(rayAngle);
