#include "simple_logger.h"

#include "gfc_input.h"
#include "gfc_list.h"
#include "gfc_audio.h"

#include "particle_effects.h"
#include "monsters.h"
#include "entity_common.h"
#include "dynamic_body.h"
#include "camera.h"
#include "level.h"
#include "player.h"

extern int __DebugMode;
extern int giveAll;

Vector2D screen = { 0 };

static Entity *_player = NULL;

void player_draw(Entity *self);
void player_think(Entity *self);
void player_update(Entity *self);
int  player_touch(Entity *self,Entity *other);
void player_stand(Entity *self);
float player_damage(Entity *self,float amount, Entity *source);
void player_die(Entity *self);
void player_action_end(Entity *self);
void player_use_equipped_item();

Entity *player_get()
{
    return _player;
}

void player_set_position(Vector2D position)
{
    if (!_player)
    {
        slog("no player loaded");
        return;
    }
    vector2d_copy(_player->position,position);
}

Vector2D player_get_position()
{
    if (!_player)
    {
        slog("no player loaded");
        return vector2d(0,0);
    }
    return _player->position;
}

Entity *player_spawn(Vector2D position,Uint32 id,SJson *args,const char *entityDef)
{
    if (_player != NULL)
    {
        vector2d_copy(_player->body.position,position);
        level_add_entity(_player);
        return NULL;
    }
    return player_new(position,id,args,entityDef);
}

Entity *player_new(Vector2D position,Uint32 id,SJson *args,const char *entityDef)
{
    Entity *self;
    
    self = gf2d_entity_new();

    if (!self)return NULL;

    self = entity_config_from_file(self,args,position,entityDef);
            
    self->think = player_think;
    self->update = player_update;
    self->touch = player_touch;
    self->damage = player_damage;
    self->draw = player_draw;
    self->die = player_die;
    self->free = player_free;
    self->actionEnd = player_action_end;
    self->position = position;
    self->body.shape = &self->shape;
    self->jumpcool = 0;
    self->jumpcount = 1;

    self->frame = 0;
    self->sprite = gf2d_sprite_load_all(
        "images/MouseIdle.png",
        32,
        32,
        1,
        0);

    self->id = 0;
    _player = self;

    player_set_position(position);

    level_add_entity(self);
    return self;
}

void player_activation_check(Entity *self)
{
    entity_activate(self);
}

void player_halt(int frames)
{
    if (!_player)return;
    _player->halt = frames;
}

void player_draw(Entity* self)
{
	Vector2D drawPosition,drawScale;
    Sprite *sprite;
    if (!self)return;
    if (!self->inuse)return;
    
    vector2d_add(drawPosition, self->position,gf2d_camera_get_offset());
    
    gf2d_sprite_render(
            self->sprite,
            drawPosition,
            NULL,
            NULL,
            NULL,
            NULL,
            NULL,
            NULL,
            (Uint32)self->frame);

}

void player_think(Entity *self)
{
    Sound *walkingsound;

    if (!self)return;
    if (self->halt < 0)return;// this is a permanent pause until unpaused

    walkingsound = gfc_sound_load("audio/Rat-Mouse-Running-A1-www.fesliyanstudios.com.mp3", 1, 2);

    if (self->halt > 0)
    {
        self->halt--;
        return;
    }
    if (gfc_input_command_down("walkleft"))
    {
        gfc_sound_play(walkingsound, 0, 1, -1, -1);
        self->state = ES_Walking;
        self->velocity = vector2d(-1,0);
    }
    if (gfc_input_command_down("walkright"))
    {
        gfc_sound_play(walkingsound, 0, 1, -1, -1);
        self->state = ES_Walking;
        self->velocity = vector2d(1,0);
    }
    if (gfc_input_command_down("jump") && (self->grounded) && (self->jumpcount > 0))
    {
        self->state = ES_Jumping;
        self->grounded = 0;
        self->jumpcount = 0;
        self->velocity = vector2d(0,-1);
        vector2d_scale(self->velocity, self->velocity, 50);
    }
    else
    {
        self->state = ES_Idle;
    }
}

void player_melee(Entity *self)
{
    if (!self)return;    
    self->connectedAttack = entity_melee_slash_update(
        self,
        self->body.position,
        self->facing,
        35,
        15,
        self->damages,
        MONSTER_LAYER,
        1);
}

void player_action_end(Entity *self)
{
    if (!self)return;
    if (self->state >= ES_Dying)return;
    self->state = ES_Idle;
    gf2d_entity_set_action(self, "idle");
}

void player_update(Entity *self)
{
    Level level = level_get();
    Vector2D top, left, right,bottom;
    Vector2D gravity;
    
    if (!self)return;

    vector2d_add(bottom, self->position, vector2d(10,20));
    vector2d_add(top, self->position, vector2d(10,0));
    vector2d_add(left, self->position, vector2d(0,10));
    vector2d_add(right, self->position, vector2d(20,10));

    if (self->position.x <= 32) self->position.x = 32;
    if (self->position.x >= 1136) self->position.x = 1136;

    self->oldPosition = self->position;

    self->frame += 0.1;
    if(self->frame >= 3) self->frame = 0;

    if(level_info_get_tile_index_at(level.info,level_position_to_tile(level.info, bottom)))
    {
        self->grounded = 1;
        self->jumpcount = 1;
    }
    else if ((!level_info_get_tile_index_at(level.info,level_position_to_tile(level.info, bottom))) && bottom.y > 715)
    {
        self->dead = 1;
    }
    else self->grounded = 0;
    
    if(self->grounded) gravity = vector2d(0, 0);
    else gravity = vector2d(0,self->body.gravity);

    vector2d_add(self->position, self->position, self->velocity);
    vector2d_add(self->body.position, self->position, self->velocity);

    gf2d_camera_set_focus(self->position);
    gf2d_camera_bind();

    //cooldowns
    if (self->state == ES_Dying)return;
    self->cooldown-= 1;
    if (self->cooldown <= 0)
    {
        self->cooldown = 0;
        self->state = ES_Idle;
    }

    if (self->body.velocity.x)
    {
        self->body.velocity.x *= 0.8;
        if (fabs(self->body.velocity.x) < 0.7)
        {
            self->body.velocity.x = 0;
        }
    }

    switch (self->state)
    {
        case ES_Idle:
            vector2d_add(self->position, self->position, gravity);
            vector2d_add(self->body.position, self->position, gravity);
            self->velocity = vector2d(0,0);
            break;
        case ES_Seeking:
            break;
        case ES_Jumping:
            if (self->jumpcool == 0)
            {
                self->state = ES_Idle;
            }
            break;
        case ES_Attacking:
            player_melee(self);
            break;
        case ES_Pain:
        case ES_Cooldown:
            break;
        case ES_Walking:
            vector2d_add(self->position, self->position, gravity);
            vector2d_add(self->body.position, self->position, gravity);
            break;
        case ES_Dying:
            return;
        case ES_Dead:
            return;
        default:
            return;
    }
    
}

int player_touch(Entity *self,Entity *other)
{
    return 0;// player does not touch
}

void player_stand(Entity *self)
{
    if (!self)return;
    self->body.shape = &self->shape;
}

float player_damage(Entity *self,float amount, Entity *source)
{
    Vector2D dir;
    const char *action;
    if (source)
    {
        dir = monster_direction_to_player(source);
    }
    else
    {
        dir.x = -self->facing.x;
        dir.y = -1;
    }
    particle_spray(
        level_get_particle_emitter(),
        self->body.position,
        dir,
        gfc_color8(255,100,100,200),//from config
        40);
    switch(self->state)
    {
        case ES_Dying:
        case ES_Dead:
        case ES_Pain:
            return 0;
        default:
            break;
    }
    if (self->damageCooldown)return 0;

    self->health -= amount;
    self->damageCooldown = 50;
    action = gf2d_entity_get_action(self);
    if ((action)&&(strcmp(action,"pain")!=0))
    {
        gf2d_entity_set_action(self,"pain");
        self->state = ES_Pain;
    }
    gf2d_entity_play_sound(self, "pain");

    //play pain sound, set pain state
    if (self->health <= 0)
    {
        self->health = 0;
        self->state = ES_Dying;
        self->die(self);
    }
    return amount;//todo factor in shields
}


void player_free(Entity *self)
{
    if (!self)return;
    level_remove_entity(self);
}

void player_dying(Entity *self)
{
    if (!self)return;
    if (gf2d_entity_get_action_frames_remaining(self))return;// play out death first
    self->think = NULL;
    self->halt = -1;
}

void player_die(Entity *self)
{
    if (!self)return;
    gf2d_entity_set_action(self,"death");
    //play a dying sound
    self->think = player_dying;
}

/*eol@eof*/