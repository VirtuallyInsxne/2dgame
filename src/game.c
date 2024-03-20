#include <SDL.h>
#include <string.h>
#include <unistd.h>

#include "simple_logger.h"

#include "gf2d_graphics.h"
#include "gf2d_sprite.h"

#include "font.h"
#include "camera.h"
#include "entity.h"
#include "player.h"
#include "level.h"

int main(int argc, char * argv[])
{
    /*variable declarations*/
    int done = 0;
    const Uint8 * keys;
    SDL_Joystick *playerOneInput;
    Uint8 mainMenu = 1;
    Sprite *sprite;

    TextLine player1Dashes;
    TextLine player2Dashes;

    Level *firstLevel; 
    Entity *playerOne;
    Entity *playerTwo;
    
    /*program initializtion*/
    init_logger("gf2d.log",0);
    slog("---==== BEGIN ====---");
    gf2d_graphics_initialize(
        "gf2d",
        1920,
        1080,
        300,
        180,
        vector4d(0,0,0,255),
        0);
    gf2d_graphics_set_frame_delay(16);
    gf2d_sprite_init(1024);
    font_init();
    entity_system_init(50);
    SDL_ShowCursor(SDL_DISABLE);
    camera_set_size(vector2d(1200,700));

    sprite = gf2d_sprite_load_image("images/backgrounds/bg_flat.png");
    
    /*demo setup*/
    firstLevel = level_load_from_json("configs/reglevels/testlevel.level");
    level_set_active_level(firstLevel);
    playerOne = entity_get_entity_by_name("playerOne");
    playerTwo = entity_get_entity_by_name("playerTwo");
    playerOneInput =  SDL_JoystickOpen(0);
    /*main game loop*/
    while(!done)
    {
        SDL_PumpEvents();   // update SDL's internal event structures
        SDL_Delay(15);
        keys = SDL_GetKeyboardState(NULL); // get the keyboard state for this frame
        font_cleanup();

        if(mainMenu)
        {
            gf2d_graphics_clear_screen();// clears drawing buffers
        // all drawing should happen betweem clear_screen and next_frame
            //backgrounds drawn first
                gf2d_sprite_draw_image(sprite,vector2d(0,0));
                font_draw_text("Press A to Start Game",FS_small, GFC_COLOR_RED ,vector2d(10,10));
                font_draw_text("Press B to Exit Game",FS_small, GFC_COLOR_RED ,vector2d(10,40));
                
                if (access("savedata.save", F_OK) != -1)
                {
                    font_draw_text("Press Start to Load Game",FS_small, GFC_COLOR_RED ,vector2d(10,70));
                }

            gf2d_graphics_next_frame();// render current draw frame and skip to the next frame

            if (SDL_JoystickGetButton(playerOneInput, 0)) 
            {
                mainMenu = 0;
                camera_set_size(vector2d(300,180));
            }
            else if(SDL_JoystickGetButton(playerOneInput, 6))
            {
                if (access("savedata.save", F_OK) != -1)
                {
                    slog("Pressing start");
                    firstLevel = level_load_from_json("savedata.save");
                    level_set_active_level(firstLevel);
                    level_setup_camera(firstLevel);
                    mainMenu = 0;
                }
            }
            else if (SDL_JoystickGetButton(playerOneInput, 1)) done = 1;
        }
        else
        {
            SDL_PumpEvents();   // update SDL's internal event structures
            SDL_Delay(15);
            keys = SDL_GetKeyboardState(NULL); // get the keyboard state for this frame
            font_cleanup();

            entity_system_think();

            entity_system_update();
        
            gf2d_graphics_clear_screen();

                level_draw(level_get_active_level());

                gfc_line_sprintf(player1Dashes, "Player One Dashes: %d", playerOne->dashes);
                gfc_line_sprintf(player2Dashes, "Player Two Dashes: %d", playerTwo->dashes);

                font_draw_text(player1Dashes, FS_small, GFC_COLOR_CYAN, vector2d(10,10));
                font_draw_text(player2Dashes, FS_small, GFC_COLOR_CYAN, vector2d(10,30));

                entity_system_draw();

            gf2d_graphics_next_frame();

            if (keys[SDL_SCANCODE_ESCAPE])
            {
                level_save_data_to_copy("configs/reglevels/testlevel.level", playerOne->position, playerTwo->position);
                done = 1;
            }
        }
    }
    level_free(firstLevel);
    slog("---==== END ====---");
    return 0;
}

/*eol@eof*/
