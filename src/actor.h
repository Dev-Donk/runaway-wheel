#ifndef ACTOR_H
#define ACTOR_H

#include "raylib.h"

typedef struct 
{
    Vector2     a_pos;
    Texture     a_texture;
    Rectangle   a_collision_box;
} actor;

void actor_move(actor *a);
void actor_move_without_collision(actor *a);

#endif