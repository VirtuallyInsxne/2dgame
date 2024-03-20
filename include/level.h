#ifndef __LEVEL_H__
#define __LEVEL_H__

#include "gfc_list.h"

#include "gf2d_sprite.h"

typedef struct
{
    Sprite      *background;
    Sprite      *tileLayer;
    Sprite      *tileSet;
    Vector2D     levelSize;
    Vector2D     tileSize;
    Uint8       *tileMap;
    List        *shapes;
}Level;

/**
 * @brief allocate space for empty level
 * @return NULL on error, empty level otherwise
*/
Level *level_new(Uint32 width, Uint32 height);

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

Level *level_get_active_level();

void level_set_active_level(Level *level);

void level_tile_layer_build(Level *level);

int level_shape_clip(Level *level, Shape shape);

void level_build_clip_space(Level *level);

void level_save_data_to_copy(const char *filename, Vector2D player1pos, Vector2D player2pos);

#endif