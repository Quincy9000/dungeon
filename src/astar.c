#include "raylib.h"
#include "tile.c"
#include "constants.c"
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>

typedef struct
{
    int x, y;
    int g, h;
    int parent; // index in node buffer
    bool open;
    bool closed;
} Node;

// maximum grid size (adjust to your map)
#define MAX_NODES (GRID_WIDTH * GRID_HEIGHT)

// reusable buffers
static Node nodesBuffer[MAX_NODES];
static Vector2 pathBuffer[MAX_NODES];

// walkable tiles lookup
static bool isWalkable[MAX_NODES];
static int walkableIndices[MAX_NODES];
static int walkableCount = 0;

/**
 * Call this once after map load to build walkable tiles.
 */
void BuildWalkableTiles(const Tile *tiles)
{
    walkableCount = 0;
    for (int y = 0; y < GRID_HEIGHT; y++)
    {
        for (int x = 0; x < GRID_WIDTH; x++)
        {
            int i = y * GRID_WIDTH + x;
            if (!(tiles[i].mask & SOLID_MASK)) // only non-solid tiles
            {
                walkableIndices[walkableCount++] = i;
                isWalkable[i] = true;
            }
            else
            {
                isWalkable[i] = false;
            }
        }
    }
    printf("Walkable tiles: %d / %d\n", walkableCount, GRID_WIDTH * GRID_HEIGHT);
}

int heuristic(int ax, int ay, int bx, int by)
{
    int dx = ax - bx;
    if (dx < 0)
        dx = -dx;
    int dy = ay - by;
    if (dy < 0)
        dy = -dy;
    return dx + dy; // Manhattan
}

/**
 * Finds a path from (sx,sy) to (gx,gy) on the tile map.
 * Returns the number of steps in outPathLength.
 * The path is written into pathBuffer and is in tile coordinates.
 * Returns true if a path is found, false otherwise.
 */
bool AStarFindPathCached(int sx, int sy, int gx, int gy, const Tile *tiles, size_t *outPathLength)
{
    const int width = GRID_WIDTH;
    const int height = GRID_HEIGHT;
    const int total = width * height;

    if (sx < 0 || sx >= width || sy < 0 || sy >= height)
        return false;
    if (gx < 0 || gx >= width || gy < 0 || gy >= height)
        return false;

    int startIdx = sy * width + sx;
    int goalIdx = gy * width + gx;

    if ((tiles[startIdx].mask & SOLID_MASK) || (tiles[goalIdx].mask & SOLID_MASK))
    {
        if (outPathLength)
            *outPathLength = 0;
        return false; // start or goal blocked
    }

    // init nodes
    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            int i = y * width + x;
            nodesBuffer[i].x = x;
            nodesBuffer[i].y = y;
            nodesBuffer[i].g = INT_MAX;
            nodesBuffer[i].h = 0;
            nodesBuffer[i].parent = -1;
            nodesBuffer[i].open = false;
            nodesBuffer[i].closed = false;
        }
    }

    nodesBuffer[startIdx].g = 0;
    nodesBuffer[startIdx].h = heuristic(sx, sy, gx, gy);
    nodesBuffer[startIdx].open = true;

    bool found = false;

    while (true)
    {
        // find open node with lowest f
        int current = -1;
        int bestF = INT_MAX;
        for (int i = 0; i < total; ++i)
        {
            if (nodesBuffer[i].open && !nodesBuffer[i].closed)
            {
                int f = nodesBuffer[i].g + nodesBuffer[i].h;
                if (f < bestF)
                {
                    bestF = f;
                    current = i;
                }
            }
        }

        if (current == -1)
            break; // no path
        if (current == goalIdx)
        {
            found = true;
            break;
        }

        nodesBuffer[current].open = false;
        nodesBuffer[current].closed = true;

        int cx = nodesBuffer[current].x;
        int cy = nodesBuffer[current].y;

        const int dirs[4][2] = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}};
        for (int d = 0; d < 4; ++d)
        {
            int nx = cx + dirs[d][0];
            int ny = cy + dirs[d][1];
            if (nx < 0 || nx >= width || ny < 0 || ny >= height)
                continue;

            int ni = ny * width + nx;

            if (!isWalkable[ni])
                continue; // skip walls
            if (nodesBuffer[ni].closed)
                continue;

            int tentativeG = nodesBuffer[current].g + 1;

            if (!nodesBuffer[ni].open || tentativeG < nodesBuffer[ni].g)
            {
                nodesBuffer[ni].g = tentativeG;
                nodesBuffer[ni].h = heuristic(nx, ny, gx, gy);
                nodesBuffer[ni].parent = current;
                nodesBuffer[ni].open = true;
                nodesBuffer[ni].closed = false; // reopen if needed
            }
        }
    }

    if (!found)
    {
        if (outPathLength)
            *outPathLength = 0;
        return false;
    }

    // reconstruct path
    size_t pathLen = 0;
    int cur = goalIdx;
    int safety = 0;
    while (cur != -1 && safety++ < MAX_NODES)
    {
        pathBuffer[pathLen++] = (Vector2){(float)nodesBuffer[cur].x, (float)nodesBuffer[cur].y};
        if (cur == startIdx)
            break;
        cur = nodesBuffer[cur].parent;
    }

    // reverse path
    for (size_t i = 0; i < pathLen / 2; ++i)
    {
        Vector2 tmp = pathBuffer[i];
        pathBuffer[i] = pathBuffer[pathLen - 1 - i];
        pathBuffer[pathLen - 1 - i] = tmp;
    }

    if (outPathLength)
        *outPathLength = pathLen;
    return true;
}
