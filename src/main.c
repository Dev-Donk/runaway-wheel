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
#define SCREEN_HEIGHT 	1024

#define SPRITE_SIZE_MULTIPLIER 8

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
	Texture game_actor_player 		= LoadTexture("player_test.png");

	// Backgrounds
	Texture game_world_sky_box 		= LoadTextureFromImage(GenImageGradientLinear(SCREEN_WIDTH, SCREEN_HEIGHT, 0, WHITE, SKYBLUE));
	Texture game_world_foreground 	= LoadTexture("foreground_test.png");
	Texture game_world_midground 	= LoadTexture("midground_test.png");
	Texture game_world_background 	= LoadTexture("background_test.png");

	// --------------------------------------------------------------

	// Game Essentialsssss
	// --------------------------------------------------------------

	// Difficulty
	float game_difficulty = 1.0f;

	// Background speeds
	float scrolling_front 	= 0.0f; // Obstacles use this variable
	float scrolling_mid 	= 0.0f;
	float scrolling_back 	= 0.0f;

	// Player
	actor player = { (Vector2){ 512.0f, 640.0f }, (Vector2){ 0.0f, 0.0f }, game_actor_player};

	// Text Loading & Prompts
	// Words
	prompt_entry *list_words	= (prompt_entry*)malloc(sizeof(prompt_entry));
	int list_words_size;
	list_words_size 			= list_load_from_text_file("words.txt", list_words);
	list_print(list_words);

	// Phrases
	prompt_entry *list_phrases 	= (prompt_entry*)malloc(sizeof(prompt_entry));
	int list_phrases_size;
	list_phrases_size 			= list_load_from_text_file("phrases.txt", list_phrases);
	list_print(list_phrases);

	printf("Words size: %d\nPhrases size: %d\n", list_words_size, list_phrases_size);

	prompt_entry *current_prompt = list_get_random_entry(list_words, list_words_size);


	// --------------------------------------------------------------

	SetTargetFPS(60); // I mean.. It's a small game, most Windows devices should support
	
	// game loop
	while (!WindowShouldClose())		// run the loop until the user presses ESCAPE or presses the Close button on the window
	{
		// Update
		// --------------------------------------------------------------
		scrolling_back 	-= 1.0f * game_difficulty;
		scrolling_mid 	-= 1.5f * game_difficulty;
		scrolling_front -= 3.0f * game_difficulty;

		if (scrolling_front <= -game_world_foreground.width * SPRITE_SIZE_MULTIPLIER) 	scrolling_front = 0;
		if (scrolling_mid 	<= -game_world_midground.width  * SPRITE_SIZE_MULTIPLIER) 	scrolling_mid = 0;
		if (scrolling_back 	<= -game_world_background.width * SPRITE_SIZE_MULTIPLIER) 	scrolling_back = 0;

		// --------------------------------------------------------------

		// Input
		// Just to test random entry getting
		if(IsKeyDown(KEY_SPACE))
		{
			current_prompt = list_get_random_entry(list_words, list_words_size);
		}

		// Drawing
		// --------------------------------------------------------------
		BeginDrawing();

		// Setup the back buffer for drawing (clear color and depth buffers)
		ClearBackground(BLACK);

		// World setting
		// --------------------------------------------------------------

		//Sky
		DrawTexture(game_world_sky_box, 0, 0, WHITE);
		// Background
		DrawTextureEx(game_world_background, 	(Vector2){ scrolling_back, 0.0f }, 0.0f, (float)SPRITE_SIZE_MULTIPLIER, WHITE);
		DrawTextureEx(game_world_background, 	(Vector2){ game_world_background.width * (float)SPRITE_SIZE_MULTIPLIER + scrolling_back, 0.0f }, 
												0.0f, (float)SPRITE_SIZE_MULTIPLIER, WHITE);
		// Midground
		DrawTextureEx(game_world_midground, 	(Vector2){ scrolling_mid, 0.0f }, 0.0f, (float)SPRITE_SIZE_MULTIPLIER, WHITE);
		DrawTextureEx(game_world_midground,		(Vector2){ game_world_midground.width * (float)SPRITE_SIZE_MULTIPLIER + scrolling_mid, 0.0f }, 
												0.0f, (float)SPRITE_SIZE_MULTIPLIER, WHITE);
		// Foregorund
		DrawTextureEx(game_world_foreground, 	(Vector2){ scrolling_front, 0.0f }, 0.0f, (float)SPRITE_SIZE_MULTIPLIER, WHITE);
		DrawTextureEx(game_world_foreground, 	(Vector2){ game_world_foreground.width * (float)SPRITE_SIZE_MULTIPLIER + scrolling_front, 0.0f }, 
												0.0f, (float)SPRITE_SIZE_MULTIPLIER, WHITE);

		// --------------------------------------------------------------

		// Actors
		DrawTextureEx(player.texture, 			(Vector2){ player.pos.x, player.pos.y}, 0.0f, (float)SPRITE_SIZE_MULTIPLIER, WHITE);

		
		// Text
		DrawText(current_prompt->data, 200, 200, 20, RED);

		// --------------------------------------------------------------

		
		// end the frame and get ready for the next one  (display frame, poll input, etc...)
		EndDrawing();
	}

	// cleanup
	// --------------------------------------------------------------

	UnloadTexture(game_world_sky_box);
	UnloadTexture(game_world_foreground);
	UnloadTexture(game_world_midground);
	UnloadTexture(game_world_background);

	UnloadTexture(game_actor_player);

	list_free(list_words);
	list_free(list_phrases);

	// --------------------------------------------------------------


	// destroy the window and cleanup the OpenGL context
	CloseWindow();
	return 0;
}
