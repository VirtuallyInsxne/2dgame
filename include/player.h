#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "entity.h"

/**
 * @brief spawn a player entity
 * @return NULL on error, otherwise a pointer to the player
 */
Entity *player_new(Bool isPLayerOne);

void player_think(Entity *self);

void player_update(Entity *self);

void player_free(Entity *self);

#endif
