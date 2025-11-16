#include "raylib.h"
#include "raymath.h"

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

void FillTiles(Tile *tiles, Texture2D texture, size_t grid_width, size_t grid_height, size_t tile_width, size_t tile_height)
{
    for (int i = 0; i < grid_width * grid_height; i++)
    {
        float x = i % grid_width;
        float y = i / grid_width;
        Vector2 position = {x * tile_width, y * tile_height};
        tiles[i] = TileCreate(position, texture);
    }
}

#endif // TILE_C