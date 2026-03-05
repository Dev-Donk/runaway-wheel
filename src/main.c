/*

Runaway Wheel Capstone Project
Author: Dante Samarco

University of Pittsburgh, Spring 2026
INFSCI 1750 - Dmitriy Babichenko

Assisted help from Dr. Thumrongsak

*/

#include <stdlib.h>

#include "raylib.h"
#include "resource_dir.h"	// utility header for SearchAndSetResourceDir

#include "typingHelper.h"
#include "actor.h"

#define SCREEN_WIDTH 	2048
#define SCREEN_HEIGHT 	1280

#define SPRITE_SIZE_MULTIPLIER 8

typedef enum 
{
    PROMPT_TYPING,
    PROMPT_SUCCESS,
    PROMPT_FAIL
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

	game_state g_state = PROMPT_TYPING;
	char key_pressed = 0;
	
	// Difficulty
	float game_difficulty = 2.0f;

	// Background speeds
	float scrolling_front 	= 0.0f; // Obstacles use this variable
	float scrolling_mid_0 	= 0.0f;
	float scrolling_mid_1 	= 0.0f;
	float scrolling_back_0 	= 0.0f;
	float scrolling_back_1 	= 0.0f;

	// Player
	actor player = { (Vector2){ 512.0f, 640.0f }, (Vector2){ 0.0f, 0.0f }, game_actor_player};

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

	prompt_entry *current_prompt = list_get_random_entry(list_words);


	// --------------------------------------------------------------

	SetTargetFPS(60); // I mean.. It's a small game, most Windows devices should support
	
	// game loop
	while (!WindowShouldClose())		// run the loop until the user presses ESCAPE or presses the Close button on the window
	{
		// Update
		// --------------------------------------------------------------

		// World
		// --------------------------------------------------------------
		scrolling_front 	-= 3.0f * game_difficulty;

		scrolling_mid_0 	-= 1.5f * game_difficulty;
		scrolling_mid_1 	-= 1.25f * game_difficulty;

		scrolling_back_0 	-= 1.0f * game_difficulty;
		scrolling_back_1 	-= 0.75f * game_difficulty;

		if (scrolling_front <= -game_world_foreground.width * SPRITE_SIZE_MULTIPLIER) 	scrolling_front = 0;

		if (scrolling_mid_0 	<= -game_world_middleground_0.width  * SPRITE_SIZE_MULTIPLIER) 	scrolling_mid_0 = 0;
		if (scrolling_mid_1 	<= -game_world_middleground_1.width  * SPRITE_SIZE_MULTIPLIER) 	scrolling_mid_1 = 0;

		if (scrolling_back_0 	<= -game_world_background_0.width * SPRITE_SIZE_MULTIPLIER) 	scrolling_back_0 = 0;
		if (scrolling_back_1 	<= -game_world_background_1.width * SPRITE_SIZE_MULTIPLIER) 	scrolling_back_1 = 0;
		// --------------------------------------------------------------

		// --------------------------------------------------------------

		// Input
		// --------------------------------------------------------------
		if(IsKeyDown(KEY_SPACE))
		{
			current_prompt = list_get_random_entry(list_words); // Test prompt retrieval
		}

		switch (g_state)
		{
		case PROMPT_TYPING:
			key_pressed = GetKeyPressed();

			if(key_pressed != 0)
				printf("%c", key_pressed);

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
		DrawTextureEx(player.texture, 			(Vector2){ player.pos.x, player.pos.y}, 0.0f, (float)SPRITE_SIZE_MULTIPLIER, WHITE);

		
		// UI
		DrawText(current_prompt->data, 200, 200, 20, RED);
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
