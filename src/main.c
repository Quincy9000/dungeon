#include "raylib.h"
#include "raymath.h"
#include <stdint.h>
#include <stdio.h>

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

const int32_t WIDTH = 800;
const int32_t HEIGHT = 800;

const int32_t GRID_WIDTH = 150;
const int32_t GRID_HEIGHT = 150;

const float_t TIME_STEP = 1.0 / 60.0;

const int TILE_WIDTH = ((float)WIDTH / (float)GRID_WIDTH);
const int TILE_HEIGHT = ((float)HEIGHT / (float)GRID_HEIGHT);

typedef struct Tile
{
    Vector2 position;
    Texture2D texture;
    bool isSolid;
    bool isVisible;
} Tile;

const char *ACTOR_NAMES[] = {
    "Goblin",
    "Orc",
    "Skeleton",
    "Zombie",
    "Troll",
    "Demon",
    "Wraith",
    "Spectre",
    "Ghoul",
    "Bandit",
    "Assassin",
    "Warlock",
    "Necromancer",
    "Vampire",
    "Werewolf",
    "Dragon",
    "Giant",
    "Elemental",
    "Slime",
    "Imp",
    "Phoenix",
    "Lich",
    "Centaur",
    "Harpy",
    "Minotaur",
    "Basilisk",
    "Chimera",
    "Gorgon",
    "Medusa",
    "Manticore",
    "Griffon",
    "Sphinx",
    "Hydra",
    "Kraken",
    "Leviathan",
    "Golem",
    "Phantom",
    "Reaper",
    "Cultist",
    "Sorcerer",
    "Paladin",
    "Ranger",
    "Rogue",
    "Barbarian",
    "Cleric",
    "Druid",
    "Monk",
    "Bard",
    "Artificer",
    "Warforged",
    "Tiefling",
    "Drow",
    "Duergar",
    "Gnoll",
    "Kobold",
    "Lizardfolk",
    "Ogre",
    "Ettin",
    "Cyclops",
    "Lamia",
    "Satyr",
    "Nymph",
    "Dryad",
    "Treant",
    "Ent",
    "Wyvern",
    "Wendigo",
    "Banshee",
    "Poltergeist",
    "Oni",
    "Yokai",
    "Kappa",
    "Rakshasa",
    "Beholder",
    "Mindflayer",
    "Choker",
    "Bugbear",
    "Mantid",
    "Nightstalker",
    "Blinkhound",
    "Frostling",
    "Blightspawn",
    "Sandworm",
    "Direwolf",
    "Thrasher",
    "Howler",
    "Stonekin",
    "Bogling",
    "Marrowfiend",
    "Scourge",
    "Carrion",
    "Bonecrawler",
    "Ravager",
    "Voidling",
    "Abyssal",
    "Dreadmaw",
    "Riftspawn",
    "Tempest",
    "Quillbeast",
    "Sunwyrm",
    "Bloodsinger",
    "Skulkraven",
    "Gravetide",
    "Rotfiend",
    "Stormgrove",
    "Ironhide",
    "Spinebreaker",
    "Glasswyrm",
    "Plaguebringer",
    "Mirelurker",
    "Brimstone",
    "Nightbane",
    "Echohound",
    "Grimling",
    "Feralborn",
    "Seadrake",
    "Thornmantis",
    "Vaporling",
    "Emberwraith",
    "Skullmaw",
    "Runebear",
    "Shardfiend",
    "Fleshshaper",
    "Silkstalker",
    "Netherfiend",
    "Deepstalker",
    "Crowcaller",
    "Rootwalker",
    "Moonstalker",
    "Orphid",
    "Lanternfly",
    "Gloomseer",
    "Torrentclaw",
    "Sporelord",
    "Iceborn",
    "Sunshrieker",
    "Grimwretch",
    "Steeljaw",
    "Glassmonger",
    "Nightweaver",
    "Bloodthorn",
    "Rimehowl",
    "Voidreaper",
    "Fangedwight",
    "Leadfoot",
    "Vilehusk",
    "Riftwatcher",
    "Zephyrling",
    "Pyrewisp",
    "Saltmaw",
    "Veilstalker",
    "Bonegnasher",
};

char *GetRandomActorName()
{
    size_t index = GetRandomValue(0, (sizeof(ACTOR_NAMES) / sizeof(ACTOR_NAMES[0])) - 1);
    return (char *)ACTOR_NAMES[index];
}

const size_t ACTOR_NAMES_COUNT = sizeof(ACTOR_NAMES) / sizeof(ACTOR_NAMES[0]);

typedef struct Stats
{
    int health;
    int attack;
    int defense;
} Stats;

typedef struct Actor
{
    Vector2 position;
    Texture2D texture;
    Color tint;
    bool moved;
    Stats stats;
    char *name;
} Actor;

typedef struct Room
{
    int x;
    int y;
    int width;
    int height;
} Room;

Stats DefaultStats()
{
    Stats stats;
    stats.health = 100;
    stats.attack = 10;
    stats.defense = 5;
    return stats;
}

Stats Fight(const Stats attacker, const Stats defender)
{
    Stats updatedDefender = defender;
    int damage = Clamp(attacker.attack - defender.defense, 0, 10000);
    updatedDefender.health -= damage;
    updatedDefender.health = Clamp(updatedDefender.health, 0, 1000);
    return updatedDefender;
}

bool ActorDead(const Actor *actor)
{
    return actor->stats.health <= 0;
}

Actor ActorCreate(float x, float y, Texture2D texture, Color tint)
{
    Actor actor;
    actor.position = (Vector2){.x = x, .y = y};
    actor.texture = texture;
    actor.tint = tint;
    actor.moved = false;
    actor.stats = DefaultStats();
    actor.name = NULL;
    return actor;
}

Actor ActorSetName(Actor actor, char *name)
{
    actor.name = name;
    return actor;
}

char *ActorGetName(const Actor *actor)
{
    if (actor->name == NULL)
    {
        return "Actor";
    }
    return actor->name;
}

void ActorDraw(const Actor *actor)
{
    if (!ActorDead(actor))
    {
        DrawTextureEx(actor->texture,
                      actor->position,
                      0, 1,
                      actor->tint);
    }
}

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
    return IsKeyPressed(KEY_SPACE);
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
                       (Vector2){TILE_WIDTH, TILE_HEIGHT},
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

    bool showMessageBox = false;

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
    SetTextureFilter(texture, TEXTURE_FILTER_POINT);

    Tile *tiles = (Tile *)MemAlloc(GRID_WIDTH * GRID_HEIGHT * sizeof(Tile));
    if (!tiles)
    {
        UnloadTexture(texture);
        UnloadImage(img);
        return 1;
    }

    FillTiles(tiles, texture);

    const size_t roomCount = 30;
    Room rooms[roomCount];

    const size_t minRoomWidth = 10;
    const size_t minRoomHeight = 10;

    const size_t maxRoomWidth = 20;
    const size_t maxRoomHeight = 15;

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
        const int guiHealthPosX = 100;
        GuiDrawText("Player Health:", (Rectangle){10, 50, 150, 30}, TEXT_ALIGN_LEFT, WHITE);
        GuiDrawText(TextFormat("%d", a.stats.health), (Rectangle){10, 70, 50, 30}, TEXT_ALIGN_LEFT, WHITE);
        GuiSetStyle(DEFAULT, TEXT_SIZE, 12);

        EndDrawing();
    }

    CloseWindow();
    MemFree(tiles);
    UnloadTexture(texture);
    UnloadImage(img);

    return 0;
}