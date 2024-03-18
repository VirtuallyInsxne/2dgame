#ifndef __LEVEL_H__
#define __LEVEL_H__

#include "gf2d_sprite.h"

typedef struct
{
    Sprite      *background;
    Sprite      *tileLayer;
    Sprite      *tileSet;
    Uint32       levelWidth;
    Uint32       levelHeight;
    Uint8       *tileMap;
}Level;

/**
 * @brief allocate space for empty level
 * @return NULL on error, empty level otherwise
*/
Level *level_new();

Level *level_load_from_json(const char *filename);

/**
 * @brief free up space taken up by level
 * @param level the level to free
*/
void level_free(Level *level);

/**
 * @brief draws level to the screen
 * @param level the level to draw
*/
void level_draw(Level *level);

void level_setup_camera(Level *level);

#endif