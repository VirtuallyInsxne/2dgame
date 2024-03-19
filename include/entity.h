#ifndef __ENTITY_H__
#define __ENTITY_H__

#include "gfc_types.h"

#include "gf2d_sprite.h"


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
    Uint8       _inuse;     /**<a flag to show if the entity is in use*/

    Sprite      *sprite;    /**<the sprite of the entity*/
    float        frame;     /**<the current frame of animation for the sprite*/
    EntityState  state;     /**<state of the entity including player states*/

    Vector2D     position;  /**<the position of the entity*/
    Vector2D     velocity;  /**<the velocity of the entity*/

    void (*think)(struct Entity_S *self);   /**<a pointer to the think function of the entity*/
    void (*update)(struct Entity_S *self);  /**<a pointer to the update function of the entity*/
    void (*free)(struct Entity_S *self);    /**<a pointer to the free function of the entity*/

    PlayerNumber playerNum;     /**<player number, can only be player one or player two*/
    
    float        health;

    void        *data;                       /**<for additional entity info*/
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
Entity *entity_new();

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

#endif
