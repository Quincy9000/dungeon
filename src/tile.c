#include "raylib.h"
#include "raymath.h"
#include "constants.c"

#ifndef TILE_C
#define TILE_C

const int VISIBILITY_MASK = 1 << 0;
const int EXPLORED_MASK = 1 << 1;
const int SOLID_MASK = 1 << 2;

typedef struct Tile
{
    Vector2 position;
    Texture2D texture;
    int mask;
} Tile;

Tile TileCreate(Vector2 position, Texture2D texture)
{
    Tile tile;
    tile.position = position;
    tile.texture = texture;
    tile.mask = SOLID_MASK | VISIBILITY_MASK;
    return tile;
}

void TileDraw(const Tile *tile)
{
    if (tile->mask & EXPLORED_MASK)
    {
        DrawTextureV(tile->texture,
                     tile->position,
                     GRAY);
    }
    else
    {
        DrawRectangleV(tile->position,
                       (Vector2){.x = tile->texture.width, .y = tile->texture.height},
                       BLACK);
    }
}

void FillTiles(Tile *tiles, Texture2D texture)
{
    for (int i = 0; i < GRID_WIDTH * GRID_HEIGHT; i++)
    {
        float x = i % GRID_WIDTH;
        float y = i / GRID_WIDTH;
        Vector2 position = {x * TILE_WIDTH, y * TILE_HEIGHT};
        tiles[i] = TileCreate(position, texture);
    }
}

#endif // TILE_C