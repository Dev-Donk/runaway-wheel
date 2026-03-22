/*

Runaway Wheel Capstone Project
Author: Dante Samarco

University of Pittsburgh, Spring 2026
INFSCI 1750 - Dmitriy Babichenko

Assisted help from Dr. Thumrongsak

*/

#include <stdlib.h>
#include <string.h>

#include "raylib.h"
#include "resource_dir.h"

#include "typingHelper.h"
#include "actor.h"

#define SCREEN_WIDTH 	(int)2048
#define SCREEN_HEIGHT 	(int)1280

#define SPRITE_SIZE_MULTIPLIER 8

#define WORLD_GROUND 			(float)1024.0f
#define WORLD_FOREGROUND_SPEED 	(float)600.0f
#define WORLD_MIDGROUND_SPEED 	(float)200.0f
#define WORLD_BACKGROUND_SPEED 	(float)100.0f

// player world ground equals 640
#define ACTOR_PLAYER_MAX_JUMP_HEIGHT 	(float)300.0f
#define ACTOR_PLAYER_ON_GROUND 		 	(float)(WORLD_GROUND - 384.0f)

#define ACTOR_OBSTACLE_ON_GROUND		(float)(WORLD_GROUND - 128.0f)


typedef enum 
{
    PROMPT_TYPING,
    PROMPT_SUCCESS,
    PROMPT_FAIL,
	PROMPT_LOAD,
	JUMP
} state;

int main ()
{
	// Tell the window to use vsync and work on high DPI displays
	SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);

	// Create the window and OpenGL context
	// Size set so textures are tilable
	InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Runaway Wheel");

	// Utility function from resource_dir.h to find the resources folder and set it as the current working directory so we can load from it
	SearchAndSetResourceDir("resources");

	// Load assets
	// --------------------------------------------------------------

	// Actors
	Texture game_actor_player 							= LoadTexture("player.png");
	Texture game_actor_obstacle_cactus					= LoadTexture("cactus.png");
	Texture game_actor_obstacle_danger_sign_0			= LoadTexture("danger_sign_0.png");

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

	// Time
	float delta_time;
	float game_timer;
	
	// Difficulty & Score
	float game_difficulty = 1.0f;
	unsigned int game_score = 0;

	// Background speeds
	float scrolling_front 	= 0.0f;
	float scrolling_mid_0 	= 0.0f;
	float scrolling_mid_1 	= 0.0f;
	float scrolling_back_0 	= 0.0f;
	float scrolling_back_1 	= 0.0f;

	// Actors
	actor player = { (Vector2){ 512.0f, ACTOR_PLAYER_ON_GROUND }, (Vector2){ 0.0f, 0.0f }, game_actor_player};
	actor obstacle = { (Vector2){ SCREEN_WIDTH, ACTOR_OBSTACLE_ON_GROUND }, (Vector2){ 0.0f, 0.0f }, game_actor_obstacle_cactus};

	// Text Loading & Prompts
	// Words
	prompt_list *list_words	= (prompt_list*)malloc(sizeof(prompt_list));
	{
		list_words->head = NULL;
		list_words->size = 0;
	}

	list_load_from_text_file(list_words, "words.txt");
	list_print(list_words);

	// Phrases
	prompt_list *list_phrases 	= (prompt_list*)malloc(sizeof(prompt_list));
	{
		list_phrases->head = NULL;
		list_phrases->size = 0;
	}

	list_load_from_text_file(list_phrases, "phrases.txt");
	list_print(list_phrases);

	prompt_entry *current_prompt;
	char *current_prompt_data;
	size_t current_prompt_location;

	char input_buffer[MAX_TOKEN_SIZE];

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
		scrolling_front 	-= (WORLD_FOREGROUND_SPEED 				* game_difficulty) * delta_time;

		scrolling_mid_0 	-= (WORLD_MIDGROUND_SPEED 				* game_difficulty) * delta_time;
		scrolling_mid_1 	-= ((WORLD_MIDGROUND_SPEED - 25.0f) 	* game_difficulty) * delta_time;

		scrolling_back_0 	-= (WORLD_BACKGROUND_SPEED 				* game_difficulty) * delta_time;
		scrolling_back_1 	-= ((WORLD_BACKGROUND_SPEED - 25.0f) 	* game_difficulty) * delta_time;

		if (scrolling_front		<= -game_world_foreground.width 		* SPRITE_SIZE_MULTIPLIER) 	scrolling_front = 0;

		if (scrolling_mid_0 	<= -game_world_middleground_0.width  	* SPRITE_SIZE_MULTIPLIER) 	scrolling_mid_0 = 0;
		if (scrolling_mid_1 	<= -game_world_middleground_1.width  	* SPRITE_SIZE_MULTIPLIER) 	scrolling_mid_1 = 0;

		if (scrolling_back_0 	<= -game_world_background_0.width 		* SPRITE_SIZE_MULTIPLIER) 	scrolling_back_0 = 0;
		if (scrolling_back_1 	<= -game_world_background_1.width 		* SPRITE_SIZE_MULTIPLIER) 	scrolling_back_1 = 0;
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

			game_timer -= 0.1 * (delta_time * 10);

			break;

		case PROMPT_SUCCESS:

			printf(" | SUCCESS!!!\n");
			game_score++;

			if(game_score % 5 == 0 && game_difficulty < 5.0f)
			{
				game_difficulty += 0.5;
			}

			player.pos.y -= ACTOR_PLAYER_MAX_JUMP_HEIGHT;

			game_state = JUMP;

			break;

		case JUMP:

			if(player.pos.y < ACTOR_PLAYER_ON_GROUND)
			{
				player.pos.y += 10.0f;
			}

			if(obstacle.pos.x > -SCREEN_WIDTH)
			{
				obstacle.pos.x -= (WORLD_FOREGROUND_SPEED * game_difficulty) * delta_time;
			}

			if(player.pos.y == ACTOR_PLAYER_ON_GROUND && obstacle.pos.x < -SCREEN_WIDTH)
			{
				obstacle.pos.x 	= SCREEN_WIDTH;
				game_state 		= PROMPT_LOAD;
			}	

			break;

		case PROMPT_FAIL:

			printf("\nFAIL!!!");

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
			game_timer 					= (current_prompt->data_length * 2) / game_difficulty;

			memset(input_buffer, 0, sizeof(input_buffer)); // Who cares if it's a lil slow! :D

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
		DrawTextureEx(player.texture, (Vector2){ player.pos.x, player.pos.y }, 0.0f, (float)SPRITE_SIZE_MULTIPLIER, WHITE);
		DrawTextureEx(obstacle.texture, (Vector2){ obstacle.pos.x, obstacle.pos.y }, 0.0f, (float)SPRITE_SIZE_MULTIPLIER, WHITE);

		
		// UI

		if(game_state != JUMP && game_state != PROMPT_LOAD)
		{
			DrawText(current_prompt->data, 200, SCREEN_HEIGHT / 2, 50, BLACK);
			DrawText(input_buffer, 200, SCREEN_HEIGHT / 2, 50, GRAY);

			DrawText(TextFormat("%05.02f", game_timer), 10, 30, 20, DARKGRAY);
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
