#include "simple_logger.h"

#include "gfc_shape.h"

#include "gf2d_draw.h"

#include "camera.h"
#include "level.h"
#include "entity.h"

typedef struct
{
    Entity  *entity_list;
    Uint32   entity_max;
}EntityManager;

void entity_system_close();

static EntityManager _entity_manager = {0}; /**<initialize a LOCAL global entity manager*/

void entity_system_init(Uint32 max)
{
    if (_entity_manager.entity_list)
    {
        slog("cannot have two instances of an entity manager!");
        return;
    }
    if (!max)
    {
        slog("cannot allocate 0 entities!");
        return;
    }
    _entity_manager.entity_list = gfc_allocate_array(sizeof(Entity), max);
    if (!_entity_manager.entity_list)
    {
        slog("failed to allocate global entity list!");
        return;
    }
    _entity_manager.entity_max = max;
    atexit(entity_system_close);
    slog("entity system initialized");
}

void entity_system_close()
{
    entity_clear_all(NULL);
    if (_entity_manager.entity_list)free(_entity_manager.entity_list);
    slog("entity system closed");
}

void entity_clear_all(Entity *ignore)
{
    int i;

    for (i = 0; i < _entity_manager.entity_max; i++)
    {
        if(&_entity_manager.entity_list[i] == ignore)continue;
        if (!_entity_manager.entity_list[i]._inuse)continue;
        entity_free(&_entity_manager.entity_list[i]);
    }
}

Entity *entity_new(char *name)
{
    int i;

    for (i = 0; i < _entity_manager.entity_max; i++)
    {
        if (_entity_manager.entity_list[i]._inuse)continue;
        memset(&_entity_manager.entity_list[i], 0, sizeof(Entity));
        _entity_manager.entity_list[i]._inuse = 1;
        _entity_manager.entity_list[i].parent = &_entity_manager.entity_list[i];
        _entity_manager.entity_list[i].name = name;

        return &_entity_manager.entity_list[i];
    }
    slog("no more available entities");
    return NULL;
}

void entity_free(Entity *self)
{
    if (!self)return;

    gf2d_sprite_free(self->sprite);
    if (self->free)self->free(self->data);
}

void entity_think(Entity *self)
{
    if (!self)return;
    if (self->think)self->think(self);
}

void entity_update(Entity *self)
{
    if (!self)return;
    if (self->update)self->update(self);
}

void entity_draw(Entity *self)
{
    Vector2D position, offset;

    if (!self)return;

    offset = camera_get_offset();
    vector2d_add(position, self->position, offset);

    if (self->sprite)
    {
        gf2d_sprite_render(
            self->sprite,
            position,
            NULL,
            NULL,
            NULL,
            NULL,
            NULL,
            NULL,
            (Uint32)self->frame);
    }
}

void entity_system_think()
{
    int i;

    for (i = 0; i < _entity_manager.entity_max; i++)
    {
        if (!_entity_manager.entity_list[i]._inuse)continue;
        entity_think(&_entity_manager.entity_list[i]);
    }
}

void entity_system_update()
{
    int i;

    for (i = 0; i < _entity_manager.entity_max; i++)
    {
        if (!_entity_manager.entity_list[i]._inuse)continue;
        entity_update(&_entity_manager.entity_list[i]);
    }
}

void entity_system_draw()
{
    int i;

    for (i = 0; i < _entity_manager.entity_max; i++)
    {
        if (!_entity_manager.entity_list[i]._inuse)continue;
        entity_draw(&_entity_manager.entity_list[i]);
    }
}

Shape entity_get_shape_after_move(Entity *self)
{
    Shape bounds = {0};

    if (!self)return bounds;

    gfc_shape_copy(&bounds,self->bounds);
    gfc_shape_move(&bounds,self->position);
    gfc_shape_move(&bounds,self->velocity);
    
    return bounds;
}

Shape entity_get_shape(Entity *self)
{
    Shape bounds = {0};

    if (!self)return bounds;

    gfc_shape_copy(&bounds,self->bounds);
    gfc_shape_move(&bounds,self->position);

    return bounds;
}