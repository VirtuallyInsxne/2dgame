#include "simple_logger.h"
#include "simple_json.h"

#include "gf2d_graphics.h"

#include "camera.h"
#include "player.h"
#include "level.h"

static Level *activeLevel = NULL;

Level *level_get_active_level()
{
    return activeLevel;
}

void level_set_active_level(Level *level)
{
    activeLevel = level;
}

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
        75 * level->tileSet->frame_w,
        45 * level->tileSet->frame_h);

    level->tileLayer->frame_w = 75 * level->tileSet->frame_w;
    level->tileLayer->frame_h = 45 * level->tileSet->frame_h;

    if (!level->tileLayer->surface)
    {
        slog("failed to create tileLayer surface");
        return;
    }

    for (i = 0; i < 45; i++)
    {
        for (j = 0; j < 75; j++)
        {
            index = j + (i * 75);

            if (level->tileMap[index] == 0)continue;

            position.x = j * level->tileSet->frame_w;
            position.y = i * level->tileSet->frame_h;
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

    level_build_clip_space(level);
}

Level *level_load_from_json(const char *filename)
{
    Level *level = NULL;

    SJson *json = NULL;
    SJson *levelJSON = NULL;
    SJson *playersJSON = NULL;
    SJson *player1, *player2;
    SJson *vertical, *horizontal;
    SJson *item;

    Entity *playerOne, *playerTwo;

    Vector2D player1pos, player2pos;

    const char *background;
    const char *tileSet;

    float player1posx, player1posy;
    float player2posx, player2posy;

    int frame_w, frame_h;
    int tileSize;
    int frames_per_line;
    int currentTileType;
    int levelWidth = 0, levelHeight = 0;
    int i, j;

    double x, y, w, h;

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
    sj_object_get_value_as_int(levelJSON, "tileSize", &tileSize);
    sj_object_get_value_as_int(levelJSON, "frame_w", &frame_w);
    sj_object_get_value_as_int(levelJSON, "frame_h", &frame_h);
    sj_object_get_value_as_int(levelJSON, "frames_per_line", &frames_per_line);

    level->tileSize = vector2d(tileSize, tileSize);

    level->tileSet = gf2d_sprite_load_all(
        tileSet,
        frame_w,
        frame_h,
        frames_per_line,
        1);

    level_tile_layer_build(level);

    playersJSON = sj_object_get_value(levelJSON, "players");

    if (!playersJSON)
    {
        slog("failed to load players object");
        sj_free(levelJSON);
        return NULL;
    }

    player1 = sj_object_get_value(playersJSON, "playerOne");
    player2 = sj_object_get_value(playersJSON, "playerTwo");

    if ((!player1) || (!player2))
    {
        slog("Failed to create player characters");
        sj_free(playersJSON);
        return NULL;
    }

    sj_object_get_value_as_float(player1, "x", &player1posx);
    sj_object_get_value_as_float(player1, "y", &player1posy);

    sj_object_get_value_as_float(player2, "x", &player2posx);
    sj_object_get_value_as_float(player2, "y", &player2posy);

    player1pos = vector2d(player1posx, player1posy);
    player2pos = vector2d(player2posx, player2posy);

    playerOne = player_new(1, level, player1pos);
    playerTwo = player_new(0, level, player2pos);

    level_setup_camera(level);

    sj_free(json);

    return level;

}

Level *level_new(Uint32 width, Uint32 height)
{    
    Level *level;

    if((!width)||(!height))
    {
        slog("cannot make a level with zero width or height");
        return NULL;
    }

    level = gfc_allocate_array(sizeof(Level),1);
    if (!level)
    {
        slog("failed to allocate space for level");
        return NULL;
    }

    level->tileMap = gfc_allocate_array(sizeof(Uint8), (height * width));
    level->levelSize.y = height;
    level->levelSize.x = width;

    level->shapes = gfc_list_new();

    return level;
}

void level_free(Level *level)
{
    if (!level)return;

    if (level->tileSet)gf2d_sprite_free(level->tileSet);
    if (level->tileLayer)gf2d_sprite_free(level->tileLayer);
    if (level->tileMap)free(level->tileMap);
    gfc_list_foreach(level->shapes,free);
    gfc_list_delete(level->shapes);
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

int level_shape_clip(Level *level, Shape shape)
{
    int i, c;
    Shape *clip;

    if (!level || !level->shapes) return 0;

    c = gfc_list_get_count(level->shapes);

    for (i = 0; i < c; i++)
    {
        clip = gfc_list_get_nth(level->shapes, i);

        if (!clip) continue;

        // Check for overlap between the given shape and the current clip shape
        if (gfc_shape_overlap(*clip, shape)) return 1; // Collision detected
    }

    return 0; // No collision detected
}


void level_build_clip_space(Level *level)
{
    Shape *shape;

    int i ,j, index = 0;

    if (!level)return;

    for (i = 0; i < 45; i++)
    {
        for (j = 0; j < 75; j++)
        {
            index = j + (i * 75);

            if (level->tileMap[index] == 0) continue;

            shape = gfc_allocate_array(sizeof(Shape), 1);

            if (!shape)continue;

            float x = (float)(j * level->tileSet->frame_w); // Adjusted based on frame width
            float y = (float)(i * level->tileSet->frame_h); // Adjusted based on frame height
            float w = (float)(level->tileSet->frame_w);     // Width of the shape
            float h = (float)(level->tileSet->frame_h);     // Height of the shape
            
            *shape = gfc_shape_rect(x, y, w, h);
            gfc_list_append(level->shapes, shape);
        }
    }
}