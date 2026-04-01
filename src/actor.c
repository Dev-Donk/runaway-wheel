
#include "actor.h"

void actor_move(actor *a, Vector2 at_speed,  char direction, float on_ground, float delta)
{
    // Handle X pos and speed
    // --------------------------------------------------------------
    // Make sure direction is between -1 & 1
    direction = direction < -1 ? -1 : direction > 1 ? 1 : direction;

    a->a_speed.x = at_speed.x;

    a->a_pos.x              += (direction * a->a_speed.x) * delta;
    a->a_collision_box.x    += (direction * a->a_speed.x) * delta;
    // --------------------------------------------------------------

    // Handle Y pos and speed
    // --------------------------------------------------------------
    if(a->a_on_ground)
    {
        a->a_speed.y = -at_speed.y;
        a->a_pos.y              += a->a_speed.y * delta;
        a->a_collision_box.y    += a->a_speed.y * delta;
        a->a_on_ground = 0;
    }

    if(a->a_pos.y < on_ground)
    {
        if(a->a_speed.y > ACTOR_GRAVITY)
        {
            a->a_speed.y = ACTOR_GRAVITY;
        }
        else
        {
            a->a_speed.y += ACTOR_GRAVITY * delta;
        }

        a->a_pos.y              += a->a_speed.y * delta;
        a->a_collision_box.y    += a->a_speed.y * delta;
    }
    else 
    {
        if(a->a_pos.y > on_ground)
        {
            a->a_pos.y              = on_ground;
            a->a_collision_box.y    = on_ground;
        }

        a->a_on_ground = 1;
    }
    // --------------------------------------------------------------
}

void actor_draw(const actor *a, float sprite_multiplier)
{
    //DrawRectangleRec(a->a_collision_box, RED);
    DrawTextureEx(a->a_texture, (Vector2){ a->a_pos.x, a->a_pos.y }, 0.0f, sprite_multiplier, WHITE);
}