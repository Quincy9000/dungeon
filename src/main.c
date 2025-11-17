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
#include "astar.c"
#include "textlog.c"

Vector2 Vector2LerpDecay(Vector2 start, Vector2 end, float speed, float decay)
{
    return Vector2Lerp(start, end, 1.0f - expf(decay * -speed));
}

bool IsKeyPressedFast(int key)
{
    return IsKeyPressed(key) || IsKeyPressedRepeat(key);
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
    if (IsKeyPressedFast(KEY_ESCAPE))
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

Vector2 ActorGetNextPosition(const Actor *actor, const Actor *player, const Tile *tiles)
{
    int sx = (int)(actor->position.x / TILE_WIDTH);
    int sy = (int)(actor->position.y / TILE_HEIGHT);

    int gx = (int)(player->position.x / TILE_WIDTH);
    int gy = (int)(player->position.y / TILE_HEIGHT);

    int dx = gx - sx;
    int dy = gy - sy;
    int dist = abs(dx) + abs(dy); // Manhattan

    if (dist <= 10)
    {
        size_t pathLength = 0;
        bool found = AStarFindPathCached(sx, sy, gx, gy, tiles, &pathLength);
        if (found && pathLength > 1)
        {
            Vector2 nextTile = pathBuffer[1];
            Vector2 newPosition = {
                .x = nextTile.x * TILE_WIDTH,
                .y = nextTile.y * TILE_HEIGHT};
            return newPosition;
        }
        else
        {
            return ActorMoveRandom(actor);
        }
    }
    else
    {
        return ActorMoveRandom(actor);
    }
}

bool CheckActorCanMoveTo(const Actor *actor, const Vector2 newPosition, const Tile *tiles)
{
    const int32_t W = GRID_WIDTH * TILE_WIDTH;
    const int32_t H = GRID_HEIGHT * TILE_HEIGHT;

    if (newPosition.x < 0 || newPosition.x >= W ||
        newPosition.y < 0 || newPosition.y >= H)
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

    return !(tile->mask & SOLID_MASK);
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

bool restart_game = true;

void CheckGameRestart()
{
    if (IsKeyPressedFast(KEY_R))
    {
        restart_game = true;
    }
}

int main()
{
    InitWindow(WIDTH, HEIGHT, "Dungeon Crawler");
    SetTargetFPS(60);
    SetWindowState(FLAG_VSYNC_HINT);

    while (restart_game == true)
    {
        restart_game = false;

        Camera2D camera;
        camera.target = (Vector2){WIDTH / 2.0f, HEIGHT / 2.0f};
        camera.offset = (Vector2){WIDTH / 2.0f, HEIGHT / 2.0f};
        camera.rotation = 0.0f;
        camera.zoom = ZOOM;

        TextLog *log = TextLogCreate(10);

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

        const size_t TILE_COUNT = GRID_WIDTH * GRID_HEIGHT;
        Tile *tiles = (Tile *)MemAlloc(TILE_COUNT * sizeof(Tile));
        if (!tiles)
        {
            UnloadTexture(texture);
            UnloadImage(img);
            return 1;
        }

        FillTiles(tiles, texture);

        Room rooms[ROOM_COUNT];

        for (size_t i = 0; i < ROOM_COUNT; i++)
        {
            Room tempRoom = RoomRandom(MIN_ROOM_WIDTH, MIN_ROOM_HEIGHT, MAX_ROOM_WIDTH, MAX_ROOM_HEIGHT);
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

                tempRoom = RoomRandom(MIN_ROOM_WIDTH, MIN_ROOM_HEIGHT, MAX_ROOM_WIDTH, MAX_ROOM_HEIGHT);
            }
            rooms[i] = tempRoom;
        }

        for (size_t i = 0; i < ROOM_COUNT; i++)
        {
            Room room = rooms[i];
            RoomCarve(room, tiles);
        }

        for (size_t i = 0; i < ROOM_COUNT; i++)
        {
            Room roomA = rooms[i];
            Room roomB = rooms[(i + 1) % ROOM_COUNT];
            CarveCorridor(roomA, roomB, tiles);
        }

        const size_t actorCount = 20;
        Actor actors[actorCount];

        for (int i = 0; i < actorCount; i++)
        {
            Room room = rooms[GetRandomValue(0, ROOM_COUNT - 1)];
            Vector2 center = RoomRandomSpot(&room);
            actors[i] = ActorCreate(center.x * TILE_WIDTH, center.y * TILE_HEIGHT, texture, RED);
            actors[i] = ActorSetName(actors[i], GetRandomActorName());
            ActorSetVisibility(&actors[i], false);
        }

        Room room = rooms[GetRandomValue(0, ROOM_COUNT - 1)];
        Vector2 center = RoomRandomSpot(&room);
        Actor a = ActorCreate(center.x * TILE_WIDTH, center.y * TILE_HEIGHT, texture, GREEN);
        a.stats = (Stats){.health = 200, .attack = 20, .defense = 5};
        a = ActorSetName(a, "Player");
        camera.target = a.position;
        ActorSetVisibility(&a, true);

        bool playerMoved = false;

        BuildWalkableTiles(tiles);

        while (!WindowShouldClose())
        {
            // Game Logic
            CheckGameRestart();

            TryExit();

            for (int k = 0; k < actorCount; k++)
            {
                Actor *other = &actors[k];
                if (ActorDead(other))
                {
                    continue;
                }
                ActorSetVisibility(other, false);
            }

            for (size_t i = 0; i < GRID_WIDTH * GRID_HEIGHT; i++)
            {
                Tile *tile = &tiles[i];
                if (tile->mask & SEEN_MASK)
                {
                    tile->mask = tile->mask & (~EXPLORED_MASK);
                }
            }

            const int rays = 360;
            const int ray_length = 50;
            for (int i = 0; i < rays; i++)
            {
                float angle = (float)i / (float)rays * (float)TAU;
                Vector2 rayDirection = {cosf(angle), sinf(angle)};
                Vector2 rayPosition = a.position;

                for (int j = 0; j < ray_length; j++)
                {
                    rayPosition.x += rayDirection.x * (TILE_WIDTH / 4);
                    rayPosition.y += rayDirection.y * (TILE_HEIGHT / 4);

                    int tileX = (int)(rayPosition.x / TILE_WIDTH);
                    int tileY = (int)(rayPosition.y / TILE_HEIGHT);

                    if (tileX < 0 || tileX >= GRID_WIDTH || tileY < 0 || tileY >= GRID_HEIGHT)
                    {
                        break;
                    }

                    for (int k = 0; k < actorCount; k++)
                    {
                        Actor *other = &actors[k];
                        if (ActorDead(other))
                        {
                            continue;
                        }
                        if (CheckCollisionPointRec(rayPosition, (Rectangle){other->position.x, other->position.y, other->texture.width, other->texture.height}))
                        {
                            ActorSetVisibility(other, true);
                        }
                    }

                    int index = tileY * GRID_WIDTH + tileX;
                    Tile *tile = &tiles[index];
                    if (tile->mask & SOLID_MASK)
                    {
                        break;
                    }

                    tile->mask = tile->mask | EXPLORED_MASK | SEEN_MASK;
                }
            }

            camera.target = Vector2LerpDecay(camera.target, a.position, 5.0f, TIME_STEP);

            if (playerMoved)
            {
                for (int i = 0; i < actorCount; i++)
                {
                    Actor *actor = &actors[i];
                    if (ActorDead(actor))
                    {
                        continue;
                    }

                    Vector2 newPosition = ActorGetNextPosition(actor, &a, tiles);
                    size_t walkedIntoPlayer = CheckActorWalkintoActor(actor, newPosition, &a, 1);
                    size_t walkedIntoOther = CheckActorWalkintoActor(actor, newPosition, actors, actorCount);
                    bool canWalk = CheckActorCanMoveTo(&actors[i], newPosition, tiles) && walkedIntoOther == (size_t)-1;

                    if (walkedIntoPlayer != (size_t)-1)
                    {
                        int healthBefore = a.stats.health;
                        a.stats = Fight(actors[i].stats, a.stats);
                        int damage = healthBefore - a.stats.health;
                        // printf("%s hits %s for %d damage\n", ActorGetName(actor), ActorGetName(&a), damage);
                        TextLogAddLine(log, TextFormat("%s hits %s for %d damage", ActorGetName(actor), ActorGetName(&a), damage));
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
                    // printf("%s hits %s for %d damage\n", ActorGetName(&a), ActorGetName(enemy), damage);
                    TextLogAddLine(log, TextFormat("%s hits %s for %d damage", ActorGetName(&a), ActorGetName(enemy), damage));
                    playerMoved = true;
                }
                else if (CheckActorCanMoveTo(&a, newPosition, tiles) && walkedIntoEnemy == (size_t)-1)
                {
                    a.position = newPosition;
                    playerMoved = true;
                }
            }

            // Rendering

            BeginDrawing();
            ClearBackground(BLACK);
            BeginMode2D(camera);

            for (int i = 0; i < GRID_WIDTH * GRID_HEIGHT; i++)
            {
                Tile *tile = &tiles[i];
                TileDraw(tile);
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
                if (actor->mask & VISIBILITY_MASK)
                {
                    GuiSetStyle(DEFAULT, TEXT_SIZE, 14);
                    GuiDrawText(TextFormat("%s", ActorGetName(actor)), (Rectangle){screenPos.x - 50, screenPos.y - 40, 100, 20}, TEXT_ALIGN_CENTER, WHITE);
                    GuiSetStyle(DEFAULT, TEXT_SIZE, 12);
                }
            }

            EndMode2D();

            DrawText("Use WASD or Arrow Keys to move, SPACE to wait, ESC to exit", 10, 10, 20, WHITE);

            GuiSetStyle(DEFAULT, TEXT_SIZE, 14);
            GuiDrawText("Player Health:", (Rectangle){10, 50, 150, 30}, TEXT_ALIGN_LEFT, WHITE);
            GuiDrawText(TextFormat("%d", a.stats.health), (Rectangle){10, 70, 50, 30}, TEXT_ALIGN_LEFT, WHITE);
            GuiSetStyle(DEFAULT, TEXT_SIZE, 12);

            DrawText(TextFormat("FPS: %d", GetFPS()), WIDTH - 100, 10, 20, WHITE);

            DrawLogs(log, (Vector2){50, HEIGHT - 100}, 20, WHITE, 100);

            EndDrawing();

            if (IsKeyPressed(KEY_C))
            {
                TextLogClear(log);
            }

            if (IsKeyPressed(KEY_L))
            {
                printf("Adding log entry\n");
                TextLogAddLine(log, "This is a test log entry.");
            }

            if (restart_game)
            {
                break;
            }
        }

        UnloadTexture(texture);
        MemFree(tiles);
    }

    CloseWindow();

    return 0;
}