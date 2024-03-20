#ifndef __ENTITY_H__
#define __ENTITY_H__

#include "gfc_types.h"
#include "gfc_vector.h"
#include "gfc_shape.h"

#include "gf2d_sprite.h"

#include "level.h"

typedef enum
{
    ES_IDLE = 0,
    ES_WALK,
    ES_DASH,
    ES_JUMP,
    ES_WALLSLIDE,
    ES_HURT,
    ES_DEATH
}EntityState;

typedef enum
{
    PLAYER_ONE,
    PLAYER_TWO
}PlayerNumber;

/**
 *@brief creates the entity struct
 */

typedef struct Entity_S
{
    Uint8                   _inuse;         /**<a flag to show if the entity is in use*/

    Level                   *level;

    char                    *name;
    
    Sprite                  *sprite;        /**<the sprite of the entity*/
    float                    frame;         /**<the current frame of animation for the sprite*/
    EntityState              state;         /**<state of the entity including player states*/

    Vector2D                 position;      /**<the position of the entity*/
    Vector2D                 velocity;      /**<the velocity of the entity*/
    Vector2D                 acceleration;
    Vector2D                 dir;

    Uint32                   jmps;
    Uint32                   jmpCooldown;
    Uint32                   dashes;
    Uint32                   dashCooldown;

    PlayerNumber             playerNum;     /**<player number, can only be player one or player two*/
    
    float                    health;

    struct Entity_S         *parent;

    Shape                    bounds;

    void                     *data;         /**<for additional entity info*/

    void (*think)(struct Entity_S *self);   /**<a pointer to the think function of the entity*/
    void (*update)(struct Entity_S *self);  /**<a pointer to the update function of the entity*/
    void (*free)(struct Entity_S *self);    /**<a pointer to the free function of the entity*/
}Entity;

/**
 * @brief initializes entity system and queues up cleanup on exit
 * @param max the maximum number of entities that can exist at the same time
 */
void entity_system_init(Uint32 max);

/**
 * @brief cleans up all active entities
 * @param ignore an entity designated not to be cleaned up
 */
void entity_clear_all(Entity *ignore);

/**
 * @brief create a blank entity for use
 * @return NULL or error if no room, a blank entity otherwise
 */
Entity *entity_new(char* name);

/**
 * @brief clean up an entity, and free up its spot in memory for further use
 * @param self the entity to free
 */
void entity_free(Entity *self);

/**
 * @brief run the think functions for all entities
 */
void entity_system_think();

/**
 * @brief run the update functions for all entities
 */
void entity_system_update();

/**
 * @brief run the draw functions for all entities
 */
void entity_system_draw();

Entity *entity_get_entity_by_name(char *name);

Shape entity_get_shape(Entity *self);

Shape entity_get_shape_after_move(Entity *self);

#endif