#include "raylib.h"
#include "raymath.h"
#include <stdint.h>
#include <stdio.h>

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

#include "tile.c"
#include "names.c"
#include "constants.c"
#include "room.c"
#include "actor.c"

bool IsKeyPressedFast(int key)
{
    return IsKeyPressedRepeat(key) || IsKeyPressed(key);
}

Vector2 TryActorMoveInput(Actor *actor)
{
    Vector2 newPosition = actor->position;
    if (IsKeyPressedFast(KEY_RIGHT) || IsKeyPressedFast(KEY_D))
    {
        newPosition.x += TILE_WIDTH;
    }
    if (IsKeyPressedFast(KEY_LEFT) || IsKeyPressedFast(KEY_A))
    {
        newPosition.x -= TILE_WIDTH;
    }
    if (IsKeyPressedFast(KEY_UP) || IsKeyPressedFast(KEY_W))
    {
        newPosition.y -= TILE_HEIGHT;
    }
    if (IsKeyPressedFast(KEY_DOWN) || IsKeyPressedFast(KEY_S))
    {
        newPosition.y += TILE_HEIGHT;
    }
    return newPosition;
}

void TryExit()
{
    if (IsKeyPressed(KEY_ESCAPE))
    {
        CloseWindow();
    }
}

Vector2 ActorMoveRandom(const Actor *actor)
{
    Vector2 newPosition = actor->position;
    int direction = GetRandomValue(0, 3);
    switch (direction)
    {
    case 0:
        newPosition.x += TILE_WIDTH;
        break;
    case 1:
        newPosition.x -= TILE_WIDTH;
        break;
    case 2:
        newPosition.y += TILE_HEIGHT;
        break;
    case 3:
        newPosition.y -= TILE_HEIGHT;
        break;
    default:
        break;
    }
    return newPosition;
}

bool CheckActorCanMoveTo(const Actor *actor, const Vector2 newPosition, const Tile *tiles)
{
    if (newPosition.x < 0 || newPosition.x >= WIDTH ||
        newPosition.y < 0 || newPosition.y >= HEIGHT)
    {
        return false;
    }

    if (newPosition.x == actor->position.x &&
        newPosition.y == actor->position.y)
    {
        return false;
    }

    int tileX = (int)(newPosition.x / TILE_WIDTH);
    int tileY = (int)(newPosition.y / TILE_HEIGHT);

    if (tileX < 0 || tileX >= GRID_WIDTH || tileY < 0 || tileY >= GRID_HEIGHT)
    {
        return false;
    }

    int index = tileY * GRID_WIDTH + tileX;
    const Tile *tile = &tiles[index];

    return !tile->isSolid;
}

bool ActorWait()
{
    return IsKeyPressed(KEY_SPACE) || IsKeyPressedRepeat(KEY_SPACE);
}

size_t CheckActorWalkintoActor(const Actor *actor, Vector2 newPosition, const Actor *otherActors, size_t actorCount)
{
    for (size_t i = 0; i < actorCount; i++)
    {
        const Actor *other = &otherActors[i];
        if (ActorDead(other))
        {
            continue;
        }
        if (actor != other)
        {
            if (Vector2Equals(newPosition, other->position))
            {
                return i;
            }
        }
    }
    return (size_t)-1;
}

int main()
{
    InitWindow(WIDTH, HEIGHT, "Dungeon Crawler");
    SetTargetFPS(60);
    SetWindowState(FLAG_VSYNC_HINT);

    Camera2D camera;
    camera.target = (Vector2){WIDTH / 2.0f, HEIGHT / 2.0f};
    camera.offset = (Vector2){WIDTH / 2.0f, HEIGHT / 2.0f};
    camera.rotation = 0.0f;
    camera.zoom = 2.0f;

    Image img = GenImageColor(TILE_WIDTH, TILE_HEIGHT, GRAY);
    if (img.data == NULL)
    {
        UnloadImage(img);
        return 1;
    }

    Texture2D texture = LoadTextureFromImage(img);
    if (texture.id == 0)
    {
        UnloadTexture(texture);
        UnloadImage(img);
        return 1;
    }

    UnloadImage(img);
    SetTextureFilter(texture, TEXTURE_FILTER_POINT);

    Tile *tiles = (Tile *)MemAlloc(GRID_WIDTH * GRID_HEIGHT * sizeof(Tile));
    if (!tiles)
    {
        UnloadTexture(texture);
        UnloadImage(img);
        return 1;
    }

    FillTiles(tiles, texture);

    Room rooms[roomCount];

    for (size_t i = 0; i < roomCount; i++)
    {
        Room tempRoom = RoomRandom(minRoomWidth, minRoomHeight, maxRoomWidth, maxRoomHeight);
        while (1)
        {
            bool intersects = false;
            for (size_t j = 0; j < i; j++)
            {
                if (RoomIntersects(tempRoom, rooms[j]))
                {
                    intersects = true;
                    break;
                }
            }

            if (!intersects)
            {
                break;
            }

            tempRoom = RoomRandom(minRoomWidth, minRoomHeight, maxRoomWidth, maxRoomHeight);
        }
        rooms[i] = tempRoom;
    }

    for (size_t i = 0; i < roomCount; i++)
    {
        Room room = rooms[i];
        RoomCarve(room, tiles);
    }

    for (size_t i = 0; i < roomCount; i++)
    {
        Room roomA = rooms[i];
        Room roomB = rooms[(i + 1) % roomCount];
        CarveCorridor(roomA, roomB, tiles);
    }

    const size_t actorCount = 10;
    Actor actors[actorCount];

    for (int i = 0; i < actorCount; i++)
    {
        Room room = rooms[GetRandomValue(0, roomCount - 1)];
        Vector2 center = RoomRandomSpot(&room);
        actors[i] = ActorCreate(center.x * TILE_WIDTH, center.y * TILE_HEIGHT, texture, BLUE);
        actors[i] = ActorSetName(actors[i], GetRandomActorName());
    }

    Room room = rooms[GetRandomValue(0, roomCount - 1)];
    Vector2 center = RoomRandomSpot(&room);
    Actor a = ActorCreate(center.x * TILE_WIDTH, center.y * TILE_HEIGHT, texture, ORANGE);
    a.stats = (Stats){.health = 200, .attack = 20, .defense = 5};
    a = ActorSetName(a, "Player");

    bool playerMoved = false;

    while (!WindowShouldClose())
    {
        TryExit();

        camera.target = Vector2Lerp(camera.target, a.position, TIME_STEP * 5.0f);

        BeginDrawing();
        BeginMode2D(camera);
        ClearBackground(BLACK);

        for (int i = 0; i < GRID_WIDTH * GRID_HEIGHT; i++)
        {
            Tile *tile = &tiles[i];
            TileDraw(tile);
        }

        if (playerMoved)
        {
            for (int i = 0; i < actorCount; i++)
            {
                Actor *actor = &actors[i];
                if (ActorDead(actor))
                {
                    continue;
                }

                Vector2 newPosition = ActorMoveRandom(actor);
                size_t walkedIntoPlayer = CheckActorWalkintoActor(actor, newPosition, &a, 1);
                size_t walkedIntoOther = CheckActorWalkintoActor(actor, newPosition, actors, actorCount);
                bool canWalk = CheckActorCanMoveTo(&actors[i], newPosition, tiles) && walkedIntoOther == (size_t)-1;

                if (walkedIntoPlayer != (size_t)-1)
                {
                    int healthBefore = a.stats.health;
                    a.stats = Fight(actors[i].stats, a.stats);
                    int damage = healthBefore - a.stats.health;
                    printf("%s hits %s for %d damage\n", ActorGetName(actor), ActorGetName(&a), damage);
                }
                else if (canWalk)
                {
                    actors[i].position = newPosition;
                }
            }
            playerMoved = false;
        }
        else if (!ActorDead(&a) && !playerMoved)
        {
            Vector2 newPosition = TryActorMoveInput(&a);
            size_t walkedIntoEnemy = CheckActorWalkintoActor(&a, newPosition, actors, actorCount);
            if (ActorWait())
            {
                playerMoved = true;
            }
            else if (walkedIntoEnemy != (size_t)-1)
            {
                Actor *enemy = &actors[walkedIntoEnemy];
                int healthBefore = enemy->stats.health;
                enemy->stats = Fight(a.stats, enemy->stats);
                int damage = healthBefore - enemy->stats.health;
                printf("%s hits %s for %d damage\n", ActorGetName(&a), ActorGetName(enemy), damage);
                playerMoved = true;
            }
            else if (CheckActorCanMoveTo(&a, newPosition, tiles) && walkedIntoEnemy == (size_t)-1)
            {
                a.position = newPosition;
                playerMoved = true;
            }
        }

        for (int i = 0; i < actorCount; i++)
        {
            ActorDraw(&actors[i]);
        }
        ActorDraw(&a);

        for (int i = 0; i < actorCount; i++)
        {
            Actor *actor = &actors[i];
            if (ActorDead(actor))
            {
                continue;
            }
            Vector2 screenPos = actor->position;
            GuiDrawText(ActorGetName(actor), (Rectangle){screenPos.x - 50, screenPos.y - 40, 100, 20}, TEXT_ALIGN_CENTER, WHITE);
        }

        EndMode2D();
        DrawText("Use WASD or Arrow Keys to move, SPACE to wait, ESC to exit", 10, 10, 20, WHITE);

        GuiSetStyle(DEFAULT, TEXT_SIZE, 18);
        GuiDrawText("Player Health:", (Rectangle){10, 50, 150, 30}, TEXT_ALIGN_LEFT, WHITE);
        GuiDrawText(TextFormat("%d", a.stats.health), (Rectangle){10, 70, 50, 30}, TEXT_ALIGN_LEFT, WHITE);
        GuiSetStyle(DEFAULT, TEXT_SIZE, 12);

        EndDrawing();
    }

    CloseWindow();
    MemFree(tiles);
    UnloadTexture(texture);

    return 0;
}