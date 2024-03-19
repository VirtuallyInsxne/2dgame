#include <SDL.h>

#include "simple_logger.h"

#include "gfc_vector.h"

#include "camera.h"
#include "player.h"

Entity *player_new(Bool isPlayerOne)
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

    self->think = player_think;
    self->update = player_update;
    self->free = player_free;

    if (isPlayerOne) self->playerNum = PLAYER_ONE;
    else self->playerNum = PLAYER_TWO;

    return self;
}

void player_think(Entity *self)
{
    // Vector2D dir = {0};
    SDL_Joystick *playerOne;
    SDL_Joystick *playerTwo;

    if (!self)return;

    playerOne = SDL_JoystickOpen(0);
    playerTwo = SDL_JoystickOpen(1);
    
    if (self->playerNum == PLAYER_ONE)
    {
        if (SDL_JoystickGetAxis(playerOne, 1) < -32700) // UP
        {
            self->state = ES_WALK;
            self->position.y -= 1;
            /*dir.x = 1;
            vector2d_normalize(&dir);
            vector2d_scale(self->velocity, dir, 3);*/
        }
        if (SDL_JoystickGetAxis(playerOne, 1) > 32700) // DOWN
        {
            self->state = ES_WALK;
            self->position.y += 1;
            /*dir.x = -1;
            vector2d_normalize(&dir);
            vector2d_scale(self->velocity, dir, 3);*/
        }
        if (SDL_JoystickGetAxis(playerOne, 0) < -32700) // LEFT
        {
            self->state = ES_WALK;
            self->position.x -= 1;
            /*dir.x = -1;
            vector2d_normalize(&dir);
            vector2d_scale(self->velocity, dir, 3);*/
        }
        if (SDL_JoystickGetAxis(playerOne, 0) > 32700) // RIGHT
        {
            self->state = ES_WALK;
            self->position.x += 1;
            /*dir.x = -1;
            vector2d_normalize(&dir);
            vector2d_scale(self->velocity, dir, 3);*/
        }
    }
    else
    {
        if (SDL_JoystickGetAxis(playerTwo, 1) < -32700) // UP
        {
            self->state = ES_WALK;
            self->position.y -= 1;
            /*dir.x = 1;
            vector2d_normalize(&dir);
            vector2d_scale(self->velocity, dir, 3);*/
        }
        if (SDL_JoystickGetAxis(playerTwo, 1) > 32700) // DOWN
        {
            self->state = ES_WALK;
            self->position.y += 1;
            /*dir.x = -1;
            vector2d_normalize(&dir);
            vector2d_scale(self->velocity, dir, 3);*/
        }
        if (SDL_JoystickGetAxis(playerTwo, 0) < -32700) // LEFT
        {
            self->state = ES_WALK;
            self->position.x -= 1;
            /*dir.x = -1;
            vector2d_normalize(&dir);
            vector2d_scale(self->velocity, dir, 3);*/
        }
        if (SDL_JoystickGetAxis(playerTwo, 0) > 32700) // RIGHT
        {
            self->state = ES_WALK;
            self->position.x += 1;
            /*dir.x = -1;
            vector2d_normalize(&dir);
            vector2d_scale(self->velocity, dir, 3);*/
        }
    }
}

void player_update(Entity *self)
{
    if (!self)return;
    self->frame += 0.075;
    
    switch(self->state) {
        case ES_IDLE:
            if (self->frame >= 3)self->frame = 0;
        default:
            if (self->frame >= 3)self->frame = 0;
    }

    camera_center_on(self->position);

}

void player_free(Entity *self)
{
    if (!self)return;
    entity_free(self);
}
