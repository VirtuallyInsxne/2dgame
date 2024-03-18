#include "simple_logger.h"
#include "simple_json.h"

#include "gf2d_graphics.h"

#include "camera.h"
#include "level.h"

void level_tile_layer_build(Level *level)
{
    int i, j, index;
    Vector2D position;
    Uint32 frame;
    
    if (!level)return;

    if (!level->tileSet)return;

    if (level->tileLayer)
    {
        gf2d_sprite_free(level->tileLayer);
    }

    level->tileLayer = gf2d_sprite_new();

    level->tileLayer->surface = gf2d_graphics_create_surface(
        level->levelWidth * level->tileSet->frame_w,
        level->levelHeight * level->tileSet->frame_h);

    level->tileLayer->frame_w = level->levelWidth * level->tileSet->frame_w;
    level->tileLayer->frame_h = level->levelHeight * level->tileSet->frame_h;

    if (!level->tileLayer->surface)
    {
        slog("failed to create tileLayer surface");
        return;
    }

    for (i = 0; i < level->levelHeight; i++)
    {
        for (j = 0; j < level->levelWidth; j++)
        {
            index = j + (i * level->levelWidth);

            if (level->tileMap[index] == 0)continue;

            position.x = j*level->tileSet->frame_w;
            position.y = i*level->tileSet->frame_h;
            frame = level->tileMap[index] - 1;

            gf2d_sprite_draw_to_surface(
                level->tileSet,
                position,
                NULL,
                NULL,
                frame,
                level->tileLayer->surface);
        }
    }

    level->tileLayer->texture = SDL_CreateTextureFromSurface(gf2d_graphics_get_renderer(),level->tileLayer->surface);

    if (!level->tileLayer->texture)
    {
        slog("failed to convert level tile layer to texture");
        return;
    }
}

Level *level_load_from_json(const char *filename)
{
    Level *level = NULL;
    SJson *json = NULL;
    SJson *levelJSON = NULL;
    SJson *vertical, *horizontal;
    SJson *item;
    const char *background;
    const char *tileSet;
    int frame_w, frame_h;
    int frames_per_line;
    int currentTileType;
    int levelWidth = 0, levelHeight = 0;
    int i, j;

    if (!filename)
    {
        slog("cannot load level without a file");
        return NULL;
    }

    json = sj_load(filename);

    if (!json)
    {
        slog("failed to load level file %s", filename);
        return NULL;
    }

    levelJSON = sj_object_get_value(json, "level");

    if (!levelJSON)
    {
        slog("%s missing 'level' object", filename);
        sj_free(json);
        return NULL;
    }

    vertical = sj_object_get_value(levelJSON, "tileMap");

    if (!vertical) {
        slog("%s missing 'tileMap'", filename);
        sj_free(json);
        return NULL;
    }

    horizontal = sj_array_get_nth(vertical, 0);
    levelHeight = sj_array_get_count(vertical);
    levelWidth = sj_array_get_count(horizontal);
    level = level_new(levelWidth, levelHeight);

    if (!level)
    {
        slog("failed to allocate space for new level for file %s", filename);
        sj_free(json);
        return NULL;
    }

    for (j = 0; j < levelHeight; j++)
    {
        horizontal = sj_array_get_nth(vertical, j);

        if (!horizontal)continue;

        for (i = 0; i < levelWidth; i++)
        {
            item = sj_array_get_nth(horizontal, i);

            if (!item)continue;

            currentTileType = 0;
            sj_get_integer_value(item, &currentTileType);
            level->tileMap[i + (j * levelWidth)] = currentTileType;
        }
    }

    background = sj_object_get_value_as_string(levelJSON, "background");
    level->background = gf2d_sprite_load_image(background);

    tileSet = sj_object_get_value_as_string(levelJSON, "tileSet");
    sj_object_get_value_as_int(levelJSON, "frame_w", &frame_w);
    sj_object_get_value_as_int(levelJSON, "frame_h", &frame_h);
    sj_object_get_value_as_int(levelJSON, "frames_per_line", &frames_per_line);

    level->tileSet = gf2d_sprite_load_all(
        tileSet,
        frame_w,
        frame_h,
        frames_per_line,
        1);

    level_tile_layer_build(level);

    level_setup_camera(level);

    sj_free(levelJSON);

    sj_free(json);

    return level;

}

Level *level_new(Uint32 levelWidth, Uint32 levelHeight)
{    
    Level *level;

    if((!levelWidth)||(!levelHeight))
    {
        slog("cannot make a world with zero width or height");
        return NULL;
    }

    level = gfc_allocate_array(sizeof(Level),1);
    if (!level)
    {
        slog("failed to allocate space for level");
        return NULL;
    }

    level->tileMap = gfc_allocate_array(sizeof(Uint8), (levelHeight * levelWidth));
    level->levelHeight = levelHeight;
    level->levelWidth = levelWidth;

    return level;
}

void level_free(Level *level)
{
    if (!level)return;

    gf2d_sprite_free(level->background);
    gf2d_sprite_free(level->tileSet);
    gf2d_sprite_free(level->tileLayer);
    free(level->tileMap); 
    free(level);
}

void level_draw(Level *level)
{
    Vector2D offset;

    if (!level)return;

    offset = camera_get_offset();

    gf2d_sprite_draw_image(level->background,vector2d(0,0));
    gf2d_sprite_draw_image(level->tileLayer, offset);
}

void level_setup_camera(Level *level)
{
    if (!level)return;

    if ((!level->tileLayer)||(!level->tileLayer->surface))
    {
        slog("no tileLayer for level");
        return;
    }

    camera_set_bounds(gfc_rect(0, 0, level->tileLayer->surface->w, level->tileLayer->surface->h));
    camera_enable_binding(1);
    camera_apply_bounds();
}