/*

Runaway Wheel Capstone Project
Author: Dante Samarco

University of Pittsburgh, Spring 2026
INFSCI 1750 - Dmitriy Babichenko

Assisted help from Dr. Thumrongsak

*/

// Includes
// --------------------------------------------------------------
// C
#include <stdlib.h>
#include <string.h>

// Raylib
#include "raylib.h"
#include "resource_dir.h"

// Game
#include "typingHelper.h"
#include "actor.h"
// --------------------------------------------------------------

// Macros
// --------------------------------------------------------------
#define SCREEN_WIDTH 							(int)2048
#define SCREEN_HEIGHT 							(int)1280

#define TEXT_PROMPT_SIZE						(float)75.0f

#define SPRITE_SIZE_MULTIPLIER 					8
#define BY_SPRITE_SIZE(a) 						(float)((a) * (SPRITE_SIZE_MULTIPLIER))

#define GAME_MIN_TIMER 							(float)2.25f
#define GAME_MAX_DIFFICULTY						(float)10.0f

#define WORLD_GROUND 							(float)1024.0f
#define WORLD_FOREGROUND_SPEED 					(float)600.0f
#define WORLD_MIDGROUND_SPEED 					(float)200.0f
#define WORLD_BACKGROUND_SPEED 					(float)100.0f

// --------------------------------------------------------------

// Types
// --------------------------------------------------------------
typedef enum 
{
    PROMPT_TYPING,
    PROMPT_SUCCESS,
    PROMPT_FAIL,
	PROMPT_LOAD,
	JUMP
} state;
// --------------------------------------------------------------

// Function Prototypes
// --------------------------------------------------------------
float set_game_timer(float p_length, float g_difficulty);
// --------------------------------------------------------------

int main ()
{
	// Tell the window to use vsync and work on high DPI displays
	SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);

	InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Runaway Wheel");

	SearchAndSetResourceDir("resources");

	// Window Icon
	Image game_icon = LoadImage("cactus.png");
	SetWindowIcon(game_icon);

	// Load assets
	// --------------------------------------------------------------

	Font font = LoadFontEx("KiwiSoda.ttf", 96, 0, 0);

	// Textures
	Texture game_actor_player 							= LoadTexture("player.png");

	Texture game_actor_obstacle_cactus					= LoadTexture("cactus.png");
	Texture game_actor_obstacle_danger_sign_0			= LoadTexture("danger_sign_0.png");

	Texture game_actor_obstacle_texts[] = { game_actor_obstacle_cactus, game_actor_obstacle_danger_sign_0 };

	// Backgrounds
	Texture game_world_sky_box 			= LoadTextureFromImage(GenImageGradientLinear(SCREEN_WIDTH, SCREEN_HEIGHT, 0, WHITE, SKYBLUE));
	Texture game_world_foreground 		= LoadTexture("world_foreground.png");
	Texture game_world_middleground_0 	= LoadTexture("world_middleground_0.png");
	Texture game_world_middleground_1 	= LoadTexture("world_middleground_1.png");
	Texture game_world_background_0 	= LoadTexture("world_background_0.png");
	Texture game_world_background_1 	= LoadTexture("world_background_1.png");

	// --------------------------------------------------------------

	// Game Essentialsssss
	// --------------------------------------------------------------

	state game_state = PROMPT_LOAD;
	char key_pressed = 0;

	unsigned char world_actor_player_hit = 0;
	Rectangle world_fail_collision_box	 = { ACTOR_PLAYER_POS_X + BY_SPRITE_SIZE(64.0f), ACTOR_PLAYER_ON_GROUND, BY_SPRITE_SIZE(2.0f), BY_SPRITE_SIZE(48.0f) };


	// Text
	float text_size_over_time;

	// Time
	float delta_time;
	float game_timer;
	
	// Difficulty & Score
	float game_difficulty 	= 1.0f;
	unsigned int game_score = 0;

	// Background speeds
	float scrolling_front 	= 0.0f;
	float scrolling_mid_0 	= 0.0f;
	float scrolling_mid_1 	= 0.0f;
	float scrolling_back_0 	= 0.0f;
	float scrolling_back_1 	= 0.0f;

	// Actors
	actor player;
	{
		player.a_pos 			= (Vector2){ ACTOR_PLAYER_POS_X, ACTOR_PLAYER_ON_GROUND };
		player.a_speed			= (Vector2){ 0.0f, 0.0f };
		player.a_texture		= game_actor_player;
		player.a_collision_box	= (Rectangle){ ACTOR_PLAYER_POS_X + BY_SPRITE_SIZE(128.0f), ACTOR_PLAYER_ON_GROUND, BY_SPRITE_SIZE(2.0f), BY_SPRITE_SIZE(48.0f) };
		player.a_on_ground		= 1;
	}
	float actor_player_jump 					= 0.0f;
	float actor_player_jump_difficulty_modifier = 0.0f;

	actor obstacle;
	{
		obstacle.a_pos 				= (Vector2){ SCREEN_WIDTH, ACTOR_OBSTACLE_ON_GROUND };
		obstacle.a_speed			= (Vector2){ 0.0f, 0.0f };
		obstacle.a_collision_box	= (Rectangle){ SCREEN_WIDTH, ACTOR_OBSTACLE_ON_GROUND, BY_SPRITE_SIZE(16.0f), BY_SPRITE_SIZE(16.0f) };
		obstacle.a_on_ground		= 1;
	}

	// Text Loading & Prompts
	prompt_entry *current_prompt;
	char *current_prompt_data;
	size_t current_prompt_location;

	char input_buffer[MAX_TOKEN_SIZE];
	
	// Words
	prompt_list *list_words	= (prompt_list*)malloc(sizeof(prompt_list));
	{
		list_words->head = NULL;
		list_words->size = 0;
	}
	list_load_from_text_file(list_words, "words.txt");
	list_print(list_words);

	// Phrases
	prompt_list *list_phrases = (prompt_list*)malloc(sizeof(prompt_list));
	{
		list_phrases->head = NULL;
		list_phrases->size = 0;
	}
	list_load_from_text_file(list_phrases, "phrases.txt");
	list_print(list_phrases);

	// --------------------------------------------------------------

	SetTargetFPS(60); // I mean.. It's a small game, most Windows devices should support
	
	// game loop
	while (!WindowShouldClose())		// run the loop until the user presses ESCAPE or presses the Close button on the window
	{
		// Update
		// --------------------------------------------------------------
		delta_time = GetFrameTime();

		// World
		// --------------------------------------------------------------
		if(!world_actor_player_hit)
		{
			scrolling_front 		-= (WORLD_FOREGROUND_SPEED 				* game_difficulty) * delta_time;

			scrolling_mid_0 		-= (WORLD_MIDGROUND_SPEED 				* game_difficulty) * delta_time;
			scrolling_mid_1 		-= ((WORLD_MIDGROUND_SPEED - 25.0f) 	* game_difficulty) * delta_time;

			scrolling_back_0 		-= (WORLD_BACKGROUND_SPEED 				* game_difficulty) * delta_time;
			scrolling_back_1 		-= ((WORLD_BACKGROUND_SPEED - 25.0f) 	* game_difficulty) * delta_time;

			if (scrolling_front		<= -game_world_foreground.width 		* SPRITE_SIZE_MULTIPLIER) 	scrolling_front = 0;

			if (scrolling_mid_0 	<= -game_world_middleground_0.width  	* SPRITE_SIZE_MULTIPLIER) 	scrolling_mid_0 = 0;
			if (scrolling_mid_1 	<= -game_world_middleground_1.width  	* SPRITE_SIZE_MULTIPLIER) 	scrolling_mid_1 = 0;

			if (scrolling_back_0 	<= -game_world_background_0.width 		* SPRITE_SIZE_MULTIPLIER) 	scrolling_back_0 = 0;
			if (scrolling_back_1 	<= -game_world_background_1.width 		* SPRITE_SIZE_MULTIPLIER) 	scrolling_back_1 = 0;
		}
		// --------------------------------------------------------------

		// --------------------------------------------------------------

		// Input
		// --------------------------------------------------------------

		// Essentially just a player state machine.. kinda
		switch (game_state)
		{
		case PROMPT_TYPING:
			key_pressed = GetCharPressed();

			if(key_pressed == *current_prompt_data && key_pressed != 0)
			{
				printf("%c", key_pressed);
				input_buffer[current_prompt_location++] = key_pressed;

				current_prompt_data++;
			}

			if(current_prompt_location >= current_prompt->data_length)
			{
				game_state = PROMPT_SUCCESS;
			}
			else if(game_timer <= 0.0f)
			{
				game_state = PROMPT_FAIL;
			}

			game_timer 			-= 0.1 * (delta_time * 10);
			text_size_over_time -= ((text_size_over_time / 0.25f) * game_difficulty) * delta_time;

			break;

		case PROMPT_SUCCESS:

			printf(" | SUCCESS!!!\n");
			game_score++;

			if(game_score % 5 == 0 && game_difficulty < GAME_MAX_DIFFICULTY)
			{
				game_difficulty += 0.5;
				actor_player_jump_difficulty_modifier += 0.025f;
			}

			game_state = JUMP;

			break;

		case JUMP:

			// Update Player
			// --------------------------------------------------------------
			if(CheckCollisionRecs(player.a_collision_box, obstacle.a_collision_box))
			{
				actor_player_jump = ACTOR_PLAYER_JUMP_SPEED + (ACTOR_PLAYER_JUMP_SPEED * actor_player_jump_difficulty_modifier);
			}
			else
			{
				actor_player_jump = 0.0f;
			}

			actor_move(&player, (Vector2){ 0.0f, actor_player_jump }, 0, ACTOR_PLAYER_ON_GROUND, delta_time);
			// --------------------------------------------------------------

			// Update Obstacle
			// --------------------------------------------------------------
			actor_move(&obstacle, (Vector2){ (WORLD_FOREGROUND_SPEED * game_difficulty), 0.0f }, -1, ACTOR_OBSTACLE_ON_GROUND, delta_time);
			// --------------------------------------------------------------

			// Check if both are done
			// --------------------------------------------------------------
			if(player.a_on_ground && obstacle.a_pos.x < BY_SPRITE_SIZE(-16))
			{
				obstacle.a_pos.x 			= SCREEN_WIDTH;
				obstacle.a_collision_box.x 	= SCREEN_WIDTH;

				game_state 					= PROMPT_LOAD;
			}	
			// --------------------------------------------------------------

			break;

		case PROMPT_FAIL:

			// Update Player
			// --------------------------------------------------------------
			if(CheckCollisionRecs(world_fail_collision_box, obstacle.a_collision_box))
			{
				world_actor_player_hit = 1;
			}

			if(world_actor_player_hit)
				actor_move(&player, (Vector2){ 0.0f, ACTOR_PLAYER_JUMP_SPEED }, 0, SCREEN_HEIGHT, delta_time);
			// --------------------------------------------------------------

			// Update Obstacle
			// --------------------------------------------------------------
			if(!world_actor_player_hit)
				actor_move(&obstacle, (Vector2){ (WORLD_FOREGROUND_SPEED * game_difficulty), 0.0f }, -1, ACTOR_OBSTACLE_ON_GROUND, delta_time);
			// --------------------------------------------------------------

			// Check if player out of frame
			// --------------------------------------------------------------
			if(world_actor_player_hit && player.a_on_ground)
			{
				exit(20);
			}	
			// --------------------------------------------------------------

			break;

		case PROMPT_LOAD:

			if(GetRandomValue(0, 1))
			{
				current_prompt = list_get_random_entry(list_words);
			} else {
				current_prompt = list_get_random_entry(list_phrases);
			}

			current_prompt_data 		= current_prompt->data;
			current_prompt_location 	= 0;
			game_timer 					= set_game_timer(current_prompt->data_length, game_difficulty);

			text_size_over_time 		= 500;

			memset(input_buffer, 0, sizeof(input_buffer)); // Who cares if it's a lil slow! :D

			obstacle.a_texture = game_actor_obstacle_texts[GetRandomValue(0, 1)];

			game_state = PROMPT_TYPING;

			break;
		
		default:
			printf("[ERROR] Invalid or Unknown game state.");
			exit(-1);
		}
		// --------------------------------------------------------------

		// Drawing
		// --------------------------------------------------------------
		BeginDrawing();

		// Setup the back buffer for drawing (clear color and depth buffers)
		ClearBackground(BLACK);

		// World
		// --------------------------------------------------------------

		//Sky
		DrawTexture(game_world_sky_box, 0, 0, WHITE);
		// Background
		DrawTextureEx(game_world_background_1, 		(Vector2){ scrolling_back_1, 0.0f }, 0.0f, (float)SPRITE_SIZE_MULTIPLIER, WHITE);
		DrawTextureEx(game_world_background_1, 		(Vector2){ game_world_background_1.width * (float)SPRITE_SIZE_MULTIPLIER + scrolling_back_1, 0.0f }, 
													0.0f, (float)SPRITE_SIZE_MULTIPLIER, WHITE);
		DrawTextureEx(game_world_background_0, 		(Vector2){ scrolling_back_0, 0.0f }, 0.0f, (float)SPRITE_SIZE_MULTIPLIER, WHITE);
		DrawTextureEx(game_world_background_0, 		(Vector2){ game_world_background_0.width * (float)SPRITE_SIZE_MULTIPLIER + scrolling_back_0, 0.0f }, 
													0.0f, (float)SPRITE_SIZE_MULTIPLIER, WHITE);
		// Midground
		DrawTextureEx(game_world_middleground_1, 	(Vector2){ scrolling_mid_1, 0.0f }, 0.0f, (float)SPRITE_SIZE_MULTIPLIER, WHITE);
		DrawTextureEx(game_world_middleground_1,	(Vector2){ game_world_middleground_1.width * (float)SPRITE_SIZE_MULTIPLIER + scrolling_mid_1, 0.0f }, 
													0.0f, (float)SPRITE_SIZE_MULTIPLIER, WHITE);
		DrawTextureEx(game_world_middleground_0, 	(Vector2){ scrolling_mid_0, 0.0f }, 0.0f, (float)SPRITE_SIZE_MULTIPLIER, WHITE);
		DrawTextureEx(game_world_middleground_0,	(Vector2){ game_world_middleground_0.width * (float)SPRITE_SIZE_MULTIPLIER + scrolling_mid_0, 0.0f }, 
													0.0f, (float)SPRITE_SIZE_MULTIPLIER, WHITE);
		// Foregorund
		DrawTextureEx(game_world_foreground, 		(Vector2){ scrolling_front, 0.0f }, 0.0f, (float)SPRITE_SIZE_MULTIPLIER, WHITE);
		DrawTextureEx(game_world_foreground, 		(Vector2){ game_world_foreground.width * (float)SPRITE_SIZE_MULTIPLIER + scrolling_front, 0.0f }, 
													0.0f, (float)SPRITE_SIZE_MULTIPLIER, WHITE);
		// --------------------------------------------------------------

		// Actors
		actor_draw(&player, (float)SPRITE_SIZE_MULTIPLIER);
		actor_draw(&obstacle, (float)SPRITE_SIZE_MULTIPLIER);

		//DrawRectangleRec(world_fail_collision_box, RED);
		
		// UI

		if(game_state != JUMP && game_state != PROMPT_LOAD)
		{
			// * (game_timer / current_prompt->data_length)
			DrawTextEx(font, current_prompt->data, (Vector2){ 250, SCREEN_HEIGHT / 2 }, (TEXT_PROMPT_SIZE + text_size_over_time), 0, BLACK);
			DrawTextEx(font, input_buffer, (Vector2){ 250, SCREEN_HEIGHT / 2 }, (TEXT_PROMPT_SIZE + text_size_over_time), 0, DARKBLUE);

			DrawText(TextFormat("%02.02f", game_timer), 10, 30, 20, DARKGRAY);
			DrawText(TextFormat("%d", game_score), 200, 30, 20, DARKGRAY);
		}

		DrawText(TextFormat("Game state: %d", game_state), 200, 50, 20, DARKGRAY);
		DrawText(TextFormat("Difficulty: %.02f", game_difficulty), 200, 70, 20, DARKGRAY);
		// --------------------------------------------------------------

		
		// end the frame and get ready for the next one  (display frame, poll input, etc...)
		EndDrawing();
	}

	// cleanup
	// --------------------------------------------------------------

	UnloadImage(game_icon);

	UnloadTexture(game_world_sky_box);
	UnloadTexture(game_world_foreground);
	UnloadTexture(game_world_middleground_0);
	UnloadTexture(game_world_middleground_1);
	UnloadTexture(game_world_background_0);
	UnloadTexture(game_world_background_1);

	UnloadTexture(game_actor_player);
	UnloadTexture(game_actor_obstacle_cactus);
	UnloadTexture(game_actor_obstacle_danger_sign_0);

	list_free(list_words);
	list_free(list_phrases);

	// --------------------------------------------------------------


	// destroy the window and cleanup the OpenGL context
	CloseWindow();
	return 0;
}

// Checks to see if less than GAME_MIN_TIMER, and returns calculated value or GAME_MIN_TIMER
float set_game_timer(float p_length, float g_difficulty) 
{
	return (p_length * 2) / g_difficulty > GAME_MIN_TIMER ? (p_length * 2) / g_difficulty : GAME_MIN_TIMER;
}