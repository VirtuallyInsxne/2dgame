#include "simple_logger.h"

#include "camera.h"

typedef struct
{
    Vector2D position;
    Vector2D size;          // Width and Height of the screen
    Rect     bounds;        // Keep the camera in these bounds
    Bool     bindCamera;    // if true keep the camera in bounds
}Camera;

static Camera _theCamera = {0};

Vector2D camera_get_position()
{
    return _theCamera.position;
}

void camera_set_position(Vector2D position)
{
    vector2d_copy(_theCamera.position, position);

    if (_theCamera.bindCamera) camera_apply_bounds();
}

Vector2D camera_get_offset()
{
    return vector2d(-_theCamera.position.x, -_theCamera.position.y);
}

void camera_set_size(Vector2D size)
{
    vector2d_copy(_theCamera.size, size);
}

void camera_set_bounds(Rect bounds)
{
    gfc_rect_copy(_theCamera.bounds, bounds);
}

void camera_enable_binding(Bool bindCamera)
{
    _theCamera.bindCamera = bindCamera;
}

void camera_apply_bounds()
{
    if (_theCamera.position.x < _theCamera.bounds.x) _theCamera.position.x = _theCamera.bounds.x;

    if (_theCamera.position.y < _theCamera.bounds.y) _theCamera.position.y = _theCamera.bounds.y;

    if ((_theCamera.position.x + _theCamera.size.x) > (_theCamera.bounds.x + _theCamera.bounds.w))
    {
        _theCamera.position.x = (_theCamera.bounds.x + _theCamera.bounds.w) - _theCamera.size.x;
    }

    if ((_theCamera.position.y + _theCamera.size.y) > (_theCamera.bounds.y + _theCamera.bounds.h))
    {
        _theCamera.position.y = (_theCamera.bounds.y + _theCamera.bounds.h) - _theCamera.size.y;
    }
}

void camera_center_on(Vector2D target)
{
    Vector2D position;
    
    position.x = target.x - (_theCamera.size.x * 0.5);
    position.y = target.y - (_theCamera.size.y * 0.5);

    camera_set_position(position);

    if (_theCamera.bindCamera) camera_apply_bounds();
}