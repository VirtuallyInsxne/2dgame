#include <SDL.h>
#include <stdio.h>

#include "simple_logger.h"

#include "gfc_input.h"
#include "gfc_audio.h"
#include "gfc_color.h"

#include "gf2d_graphics.h"
#include "gf2d_sprite.h"
#include "gf2d_draw.h"

#include "level.h"
#include "font.h"
#include "space.h"
#include "collision.h"
#include "entity.h"
#include "camera.h"
#include "player.h"

void init_all(int argc, char *argv[]);

static int _done = 0;
int fpsMode = 0;
int editorMode = 0;
extern int __DebugMode;

int main(int argc, char * argv[])
{
    /*variable declarations*/
    LevelInfo *linfo = NULL;
    Entity *player = NULL;
    Sound *sound;
    Uint8 deadFlag;
    init_all(argc, argv);

    SDL_ShowCursor(SDL_DISABLE);
    // game specific setup

    linfo = level_info_load("levels/reglevels/testlevel.level");
    level_init(linfo, 1, editorMode);

    player = player_new(vector2d(70, 600),
               0,
               NULL,
               "entities/player/player.def");

    sound = gfc_sound_load("audio/honor-and-sword-main-11222.mp3", 0.01, 1);
    gfc_sound_play(sound, 0, 0.2, 1, -1);

    /*main game loop*/
    while(!_done)
    {
        /*update things here*/
        gfc_input_update();
        gf2d_entity_think_all();
        gf2d_entity_update_all();

        if (player->dead) deadFlag = 1;
        
        gf2d_graphics_clear_screen();// clears drawing buffers
        // all drawing should happen betweem clear_screen and next_frame
            //backgrounds drawn first
            level_draw();    
            gf2d_entity_draw_all();
            //UI elements last
        gf2d_graphics_next_frame();// render current draw frame and skip to the next frame
        
        if (deadFlag) {
                gf2d_camera_set_position(vector2d(0,0));
                gf2d_font_draw_line_tag("YOU DIED.\nPress Enter to start again or Escape to quit",FT_H1, GFC_COLOR_LIGHTRED, vector2d(100, 100));
                player_free(player);
                if (gfc_input_command_down("jump")) 
                {
                    player = player_new(
                        vector2d(70, 600),
                        0,
                        NULL,
                        "entities/player/player.def");
                    gf2d_camera_set_focus(player->position);
                    deadFlag = 0;
                }
                if (gfc_input_command_down("cancel"))_done = 1;
        }

        if (gfc_input_command_down("cancel"))_done = 1;
        if (fpsMode)slog("Rendering at %f FPS",gf2d_graphics_get_frames_per_second());
    }
    player_free(player);
    level_clear();
    level_info_free(linfo);
    slog("---==== END ====---");
    return 0;
}

void init_all(int argc, char *argv[])
{
    int i;
    int fullscreen = 0;
    for (i = 1; i < argc; i++)
    {
        if (strcmp(argv[i],"--fullscreen") == 0)
        {
            fullscreen = 1;
        }
        else if (strcmp(argv[i],"--edit") == 0)
        {
            editorMode = 1;
        }
        else if (strcmp(argv[i],"--fps") == 0)
        {
            fpsMode = 1;
        }
        else if (strcmp(argv[i],"--debug") == 0)
        {
            __DebugMode = 1;
        }
    }
    /*program initializtion*/
    init_logger("gf2d.log",0);
    slog("---==== BEGIN ====---");
    gf2d_graphics_initialize(
        "Dungeon Quest",
        1200,
        720,
        300,
        180,
        vector4d(0,0,0,255),
        fullscreen);
    gf2d_graphics_set_frame_delay(16);
    gfc_audio_init(256,16,4,2,1,1);
    gf2d_sprite_init(1024);
    gf2d_actor_init(128);
    gf2d_armature_init(1024);
    gf2d_font_init("configs/font.cfg");
    gfc_input_init("configs/input.cfg");
    gf2d_entity_system_init(1024);
    gf2d_figure_init(1024);
    gf2d_camera_set_dimensions(0,0,300,180);
        
    SDL_ShowCursor(SDL_DISABLE);
}

/*eol@eof*/