#ifndef ACTOR_H
#define ACTOR_H

#include "raylib.h"

typedef struct 
{
    Vector2     a_pos;
    Texture     a_texture;
    Rectangle   a_collision_box;
} actor;


#endif