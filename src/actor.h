#ifndef ACTOR_H
#define ACTOR_H

#include "raylib.h"

#define ACTOR_GRAVITY (float)980.0f

#define ACTOR_PLAYER_POS_X						BY_SPRITE_SIZE(64.0f)
#define ACTOR_PLAYER_ON_GROUND 		 			(WORLD_GROUND - BY_SPRITE_SIZE(48.0f))
#define ACTOR_PLAYER_JUMP_SPEED					(float)1000.0f

#define ACTOR_OBSTACLE_ON_GROUND				(WORLD_GROUND - BY_SPRITE_SIZE(16.0f))

typedef struct 
{
    Rectangle       a_collision_box;
    Vector2         a_pos;
    Vector2         a_speed;
    Texture         a_texture;
    unsigned char   a_on_ground;
} actor;

void actor_move(actor *a, Vector2 at_speed,  char direction, float on_ground, float delta);
Texture actor_get_random_texture_from_array(const Texture *arr[]);
void actor_draw(const actor *a, float sprite_multiplier);

#endif