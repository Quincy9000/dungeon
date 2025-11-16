#include "raylib.h"
#include "raymath.h"

#include "constants.c"
#include "tile.c"

typedef struct Room
{
    int x;
    int y;
    int width;
    int height;
} Room;

Room RoomCreate(int x, int y, int width, int height)
{
    Room room;
    room.x = x;
    room.y = y;
    room.width = width;
    room.height = height;
    return room;
}

Vector2 RoomCenter(const Room room)
{
    Vector2 center;
    center.x = (int)(room.x + room.width / 2);
    center.y = (int)(room.y + room.height / 2);
    return center;
}

Vector2 RoomRandomSpot(const Room *room)
{
    Vector2 spot;
    spot.x = GetRandomValue(room->x, room->x + room->width - 1);
    spot.y = GetRandomValue(room->y, room->y + room->height - 1);
    return spot;
}

int RoomArea(const Room room)
{
    return room.width * room.height;
}

int RoomLeft(const Room room)
{
    return room.x;
}

int RoomRight(const Room room)
{
    return room.x + room.width;
}

int RoomTop(const Room room)
{
    return room.y;
}

int RoomBottom(const Room room)
{
    return room.y + room.height;
}

bool RoomIntersects(const Room a, const Room b)
{
    return (RoomLeft(a) < RoomRight(b) &&
            RoomRight(a) > RoomLeft(b) &&
            RoomTop(a) < RoomBottom(b) &&
            RoomBottom(a) > RoomTop(b));
}

Room RoomRandom(int minWidth, int minHeight, int maxWidth, int maxHeight)
{
    int width = GetRandomValue(minWidth, maxWidth);
    int height = GetRandomValue(minHeight, maxHeight);
    int x = GetRandomValue(0, GRID_WIDTH - width - 1);
    int y = GetRandomValue(0, GRID_HEIGHT - height - 1);
    return RoomCreate(x, y, width, height);
}

void RoomCarve(const Room room, Tile *tiles)
{
    for (int i = 0; i < room.width; i++)
    {
        for (int j = 0; j < room.height; j++)
        {
            int tileX = room.x + i;
            int tileY = room.y + j;
            if (tileX >= 0 && tileX < GRID_WIDTH && tileY >= 0 && tileY < GRID_HEIGHT)
            {
                int index = tileY * GRID_WIDTH + tileX;
                Tile *tile = &tiles[index];
                tile->isSolid = false;
                tile->isVisible = false;
            }
        }
    }
}

void CarveCorridor(const Room roomA, const Room roomB, Tile *tiles)
{
    int x1 = roomA.x + roomA.width / 2;
    int y1 = roomA.y + roomA.height / 2;
    int x2 = roomB.x + roomB.width / 2;
    int y2 = roomB.y + roomB.height / 2;

    int currentX = x1;
    int currentY = y1;

    while (currentX != x2)
    {
        int index = currentY * GRID_WIDTH + currentX;
        Tile *tile = &tiles[index];
        tile->isSolid = false;
        tile->isVisible = false;

        currentX += (x2 > currentX) ? 1 : -1;
    }

    while (currentY != y2)
    {
        int index = currentY * GRID_WIDTH + currentX;
        Tile *tile = &tiles[index];
        tile->isSolid = false;
        tile->isVisible = false;

        currentY += (y2 > currentY) ? 1 : -1;
    }
}