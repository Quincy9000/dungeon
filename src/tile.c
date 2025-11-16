#include "raylib.h"
#include "raymath.h"
#include "constants.c"

#ifndef TILE_C
#define TILE_C

typedef struct Tile
{
    Vector2 position;
    Texture2D texture;
    bool isSolid;
    bool isVisible;
} Tile;

Tile TileCreate(Vector2 position, Texture2D texture)
{
    Tile tile;
    tile.position = position;
    tile.texture = texture;
    tile.isSolid = true;
    tile.isVisible = true;
    return tile;
}

void TileDraw(const Tile *tile)
{
    if (tile->isVisible)
    {
        DrawTextureV(tile->texture,
                     tile->position,
                     WHITE);
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