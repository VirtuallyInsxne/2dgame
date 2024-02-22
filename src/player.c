#include <SDL.h>

#include "simple_logger.h"
#include "gfc_vector.h"

#include "player.h"

void player_think(Entity *self);

void player_update(Entity *self);

void player_free(Entity *self);

Entity *player_new()
{
    Entity *self;
    self = entity_new();

    if (!self)
    {
        slog("failed to spawn a player entity!");
        return NULL;
    }

    self->sprite = gf2d_sprite_load_all(
    "images/dino_models/male/kira/ghost/idle.png",
    24,
    24,
    3,
    0);
    self->state = ES_IDLE;
    self->frame = 0;
    self->position = vector2d(600,350);

    self->think = player_think;
    self->update = player_update;
    self->free = player_free;

    return self;
}

void player_think(Entity *self)
{
    Vector2D dir = {0};
    if (!self)return;
    const Uint8 *keys;
    keys = SDL_GetKeyboardState(NULL);
    if (keys[SDL_SCANCODE_D])
    {
        slog("moving right");
        self->state = ES_WALK;
        self->position.x += 3;
        /*dir.x = 1;
        vector2d_normalize(&dir);
        vector2d_scale(self->velocity, dir, 3);*/
    }
    if (keys[SDL_SCANCODE_A])
    {
        slog("moving left");
        self->state = ES_WALK;
        self->position.x -= 3;
        /*dir.x = -1;
        vector2d_normalize(&dir);
        vector2d_scale(self->velocity, dir, 3);*/
    }
}

void player_update(Entity *self)
{
    if (!self)return;
    self->frame += 0.1;
    if (self->frame >= 3)self->frame = 0;

    //vector2d_add(self->position, self->position, self->velocity);
}

void player_free(Entity *self)
{
    if (!self)return;
    entity_free(self);
}
