#include "raylib.h"
#include "raymath.h"
#include <stdint.h>
#include <stdio.h>

#ifndef ACTOR_C
#define ACTOR_C

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
    int mask;
} Actor;

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
    actor.mask = 0;
    return actor;
}

void ActorSetVisibility(Actor *actor, bool visible)
{
    if (visible)
    {
        actor->mask = actor->mask | VISIBILITY_MASK;
    }
    else
    {
        actor->mask = actor->mask & (~VISIBILITY_MASK);
    }
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
    if (!ActorDead(actor) && actor->mask & VISIBILITY_MASK)
    {
        DrawTextureEx(actor->texture,
                      actor->position,
                      0, 1,
                      actor->tint);
    }
}

#endif // ACTOR_C