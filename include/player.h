#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "gfc_vector.h"

#include "entity.h"

/**
 * @brief spawn a player entity
 * @return NULL on error, otherwise a pointer to the player
 */
Entity *player_new(Bool isPLayerOne, Level *level, Vector2D position);

void player_think(Entity *self);

void player_update(Entity *self);

void player_free(Entity *self);

Rect get_tile_rect(int tileX, int tileY, Uint32 tileWidth, Uint32 tileHeight);

void check_tile_collision(Entity *self);

#endif
