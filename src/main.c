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

#define WORLD_GROUND (float)640.0f
#define ACTOR_PLAYER_MAX_JUMP_HEIGHT (float)300.0f


typedef enum 
{
    PROMPT_TYPING,
    PROMPT_SUCCESS,
    PROMPT_FAIL,
	PROMPT_LOAD,
	JUMP
} game_state;

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
	Texture game_actor_player 			= LoadTexture("player.png");

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

	game_state g_state = PROMPT_LOAD;
	char key_pressed = 0;

	float delta_time;
	
	// Difficulty
	float game_difficulty = 2.0f;

	// Background speeds
	float scrolling_front 	= 0.0f; // Obstacles use this variable
	float scrolling_mid_0 	= 0.0f;
	float scrolling_mid_1 	= 0.0f;
	float scrolling_back_0 	= 0.0f;
	float scrolling_back_1 	= 0.0f;

	// Player
	actor player = { (Vector2){ 512.0f, WORLD_GROUND }, (Vector2){ 0.0f, 0.0f }, game_actor_player};

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

	float time_left;
	unsigned int counter = 0;


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
		scrolling_front 	-= (300.0f * game_difficulty) * delta_time;

		scrolling_mid_0 	-= (70.0f * game_difficulty) * delta_time;
		scrolling_mid_1 	-= (50.0f * game_difficulty) * delta_time;

		scrolling_back_0 	-= (30.0f * game_difficulty) * delta_time;
		scrolling_back_1 	-= (10.0f * game_difficulty) * delta_time;

		if (scrolling_front		<= -game_world_foreground.width * SPRITE_SIZE_MULTIPLIER) 	scrolling_front = 0;

		if (scrolling_mid_0 	<= -game_world_middleground_0.width  * SPRITE_SIZE_MULTIPLIER) 	scrolling_mid_0 = 0;
		if (scrolling_mid_1 	<= -game_world_middleground_1.width  * SPRITE_SIZE_MULTIPLIER) 	scrolling_mid_1 = 0;

		if (scrolling_back_0 	<= -game_world_background_0.width * SPRITE_SIZE_MULTIPLIER) 	scrolling_back_0 = 0;
		if (scrolling_back_1 	<= -game_world_background_1.width * SPRITE_SIZE_MULTIPLIER) 	scrolling_back_1 = 0;
		// --------------------------------------------------------------

		// --------------------------------------------------------------

		// Input
		// --------------------------------------------------------------

		// Essentially just a player state machine.. kinda
		switch (g_state)
		{
		case PROMPT_TYPING:
			key_pressed = GetCharPressed();

			if(key_pressed == *current_prompt_data && key_pressed != 0)
			{
				printf("%c", key_pressed);
				input_buffer[current_prompt_location] = key_pressed;

				current_prompt_data++;
				current_prompt_location++;
			}

			if(current_prompt_location >= current_prompt->data_length)
			{
				g_state = PROMPT_SUCCESS;
			}
			else if(time_left <= 0.0f)
			{
				g_state = PROMPT_FAIL;
			}

			time_left -= 0.1;

			break;

		case PROMPT_SUCCESS:

			printf(" | SUCCESS!!!\n");
			counter++;

			player.pos.y -= ACTOR_PLAYER_MAX_JUMP_HEIGHT;

			g_state = JUMP;

			break;

		case JUMP:

			if(player.pos.y < WORLD_GROUND)
			{
				player.pos.y += 10.0f;
			}
			else
			{
				g_state = PROMPT_LOAD;
			}	

			break;

		case PROMPT_FAIL:

			printf(" | FAIL!!!");

			break;

		case PROMPT_LOAD:

			current_prompt 				= list_get_random_entry(list_words); // Test prompt retrieval
			current_prompt_data 		= current_prompt->data;
			current_prompt_location 	= 0;
			time_left 					= 100.0f; // Test value

			memset(input_buffer, 0, sizeof(input_buffer)); // Who cares if it's a lil slow! :D

			g_state = PROMPT_TYPING;

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

		
		// UI
		DrawText(current_prompt->data, 200, 200, 50, DARKGRAY);
		DrawText(input_buffer, 200, 200, 50, RED);


		DrawText(TextFormat("%02.02f", time_left), 10, 30, 20, DARKGRAY);
		DrawText(TextFormat("%d", counter), 200, 30, 20, DARKGRAY);
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

	list_free(list_words);
	list_free(list_phrases);

	// --------------------------------------------------------------


	// destroy the window and cleanup the OpenGL context
	CloseWindow();
	return 0;
}
