#include <SDL.h>

#include "simple_logger.h"

#include "gfc_vector.h"

#include "camera.h"
#include "level.h"
#include "player.h"

Entity *player_new(Bool isPlayerOne, Level *level, Vector2D position)
{
    Entity *self;

    if (isPlayerOne)
    {
        self = entity_new("playerOne");
        self->sprite = gf2d_sprite_load_all(
        "images/dino_models/male/kira/ghost/idle.png",
        24,
        24,
        3,
        0);
        self->state = ES_IDLE;
        self->frame = 0;
        self->parent = self;
        self->bounds = gfc_shape_circle(0,0, 15);
        self->position = position;
        self->level = level;
        self->jmps = 2;
        self->jmpCooldown = 0;
        self->dashes = 1;
        self->velocity.y = 1;

        self->think = player_think;
        self->update = player_update;
        self->free = player_free;

        self->playerNum = PLAYER_ONE;
    }
    else
    {
        self = entity_new("playerTwo");
        self->sprite = gf2d_sprite_load_all(
        "images/dino_models/male/doux/ghost/idle.png",
        24,
        24,
        3,
        0);
        self->state = ES_IDLE;
        self->frame = 0;
        self->parent = self;
        self->bounds = gfc_shape_circle(0,0, 15);
        self->position = vector2d(96, 615);
        self->level = level;
        self->jmps = 2;
        self->jmpCooldown = 0;
        self->dashes = 1;
        self->velocity.y = 1;

        self->think = player_think;
        self->update = player_update;
        self->free = player_free;

        self->playerNum = PLAYER_TWO;
    }

    if (!self)
    {
        slog("failed to spawn a player entity!");
        return NULL;
    }

    return self;
}

void player_think(Entity *self)
{
    Vector2D dir = {0};
    SDL_Joystick *playerOne;
    SDL_Joystick *playerTwo;

    if (!self)return;

    playerOne = SDL_JoystickOpen(0);
    playerTwo = SDL_JoystickOpen(1);
    
    if (self->playerNum == PLAYER_ONE)
    {
        if (SDL_JoystickGetAxis(playerOne, 0) < -32700) // LEFT
        {
            self->state = ES_WALK;
            dir.x = -1;
            vector2d_normalize(&dir);
            vector2d_scale(self->velocity, dir, 2);
        }
        else if (SDL_JoystickGetAxis(playerOne, 0) > 32700) // RIGHT
        {
            self->state = ES_WALK;
            dir.x = 1;
            vector2d_normalize(&dir);
            vector2d_scale(self->velocity, dir, 2);
        }
        else if(SDL_JoystickGetButton(playerOne, 0) && self->jmpCooldown == 0 && self->jmps > 0) // jumps
        {
            self->state = ES_JUMP;
            self->jmps -= 1;
            dir.y = -1;
            vector2d_normalize(&dir);
            vector2d_scale(self->velocity, dir, 10);
        }
        else if(SDL_JoystickGetButton(playerOne, 2) && self->dashes > 0) // dashes
        {
            slog("Pressing X");
            self->state = ES_DASH;
            self->dashes -= 1;
            dir.x = 1;
            vector2d_normalize(&dir);
            vector2d_scale(self->velocity, dir, 10);
        }
        else
        {
            self->state = ES_IDLE;
        }
    }
    else
    {
        if (SDL_JoystickGetAxis(playerTwo, 0) < -32700) // LEFT
        {
            self->state = ES_WALK;
            dir.x = -1;
            vector2d_normalize(&dir);
            vector2d_scale(self->velocity, dir, 2);
        }
        else if (SDL_JoystickGetAxis(playerTwo, 0) > 32700) // RIGHT
        {
            self->state = ES_WALK;
            dir.x = 1;
            vector2d_normalize(&dir);
            vector2d_scale(self->velocity, dir, 2);
        }
        else if(SDL_JoystickGetButton(playerTwo, 0) && self->jmpCooldown == 0 && self->jmps > 0) // jumps
        {
            self->state = ES_JUMP;
            self->jmps -= 1;
            dir.y = -1;
            vector2d_normalize(&dir);
            vector2d_scale(self->velocity, dir, 10);
        }
        else if(SDL_JoystickGetButton(playerTwo, 2) && self->dashes > 0) // dashes
        {
            slog("Pressing X");
            self->state = ES_DASH;
            self->dashes -= 1;
            dir.x = 1;
            vector2d_normalize(&dir);
            vector2d_scale(self->velocity, dir, 10);
        }
        else
        {
            self->state = ES_IDLE;
        }
    }
}

void player_update(Entity *self)
{
    Vector2D gravity;

    gravity = vector2d(0, 1);

    if (!self)return;

    self->frame += 0.075;

    if (self->frame >= 3)self->frame = 0;

    if (level_shape_clip(level_get_active_level(), entity_get_shape_after_move(self)))
    {
        if (self->position.y >= 636) self->position.y = 636;

        self->jmps = 2;
        self->dashes = 1;
        return;
    }
    
    switch(self->state) {
        case ES_IDLE:
            if (self->jmpCooldown > 0) self->jmpCooldown -= 1;
            vector2d_add(self->velocity, self->velocity, gravity);
            vector2d_add(self->position, self->position, self->velocity);
            break;
        case ES_WALK:
            if (self->jmpCooldown > 0) self->jmpCooldown -= 1;
            vector2d_add(self->velocity, self->velocity, gravity);
            vector2d_add(self->position, self->position, self->velocity);
            break;
        case ES_JUMP:
            self->jmpCooldown = 10;
            break;
        case ES_DASH:
            vector2d_sub(self->velocity, self->velocity, gravity);
            vector2d_add(self->position, self->position, self->velocity);
            break;
        case ES_DEATH:
            self->sprite = gf2d_sprite_load_all(
                "images/dino_models/male/kira/base/dead.png",
                24,
                24,
                5,
                0);
            break;
        default:
            break;
    }

    camera_center_on(self->position);

}

void player_free(Entity *self)
{
    if (!self)return;
    entity_free(self);
}



