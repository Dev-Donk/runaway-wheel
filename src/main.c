/*
Raylib example file.
This is an example main file for a simple raylib project.
Use this as a starting point or replace it with your code.

by Jeffery Myers is marked with CC0 1.0. To view a copy of this license, visit https://creativecommons.org/publicdomain/zero/1.0/

*/

#include "raylib.h"
#include "resource_dir.h"	// utility header for SearchAndSetResourceDir

#include "typingHelper.h"
#include "actor.h"

int main ()
{
	// Tell the window to use vsync and work on high DPI displays
	SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);

	// Create the window and OpenGL context
	// Size set so textures are tilable
	InitWindow(2048, 1024, "Runaway Wheel");

	// Utility function from resource_dir.h to find the resources folder and set it as the current working directory so we can load from it
	SearchAndSetResourceDir("resources");

	// Load textures
	Texture game_foreground = LoadTexture("foreground_test.png");
	Texture game_midground = LoadTexture("midground_test.png");
	Texture game_background = LoadTexture("background_test.png");

	// Background speeds
	float scrolling_front 	= 0.0f;
	float scrolling_mid 	= 0.0f;
	float scrolling_back 	= 0.0f;

	// Game Difficulty
	float game_difficulty = 1.0f;

	SetTargetFPS(60); // I mean.. It's a small game, most Windows devices should support
	
	// game loop
	while (!WindowShouldClose())		// run the loop until the user presses ESCAPE or presses the Close button on the window
	{
		// Update
		// --------------------------------------------------------------
		scrolling_back 	-= 1.0f * game_difficulty;
		scrolling_mid 	-= 1.5f * game_difficulty;
		scrolling_front -= 3.0f * game_difficulty;

		// Textures is scaled twice it's size
		if (scrolling_front <= -game_foreground.width * 2) scrolling_front = 0;
		if (scrolling_mid <= -game_midground.width * 2) scrolling_mid = 0;
		if (scrolling_back <= -game_background.width * 2) scrolling_back = 0;

		// --------------------------------------------------------------

		// Drawing
		// --------------------------------------------------------------
		BeginDrawing();

		// Setup the back buffer for drawing (clear color and depth buffers)
		ClearBackground(BLACK);

		// World setting
		// Background
		DrawTextureEx(game_background, (Vector2){ scrolling_back, 0 }, 0.0f, 2.0f, WHITE);
		DrawTextureEx(game_background, (Vector2){ game_background.width * 2 + scrolling_back, 0 }, 0.0f, 2.0f, WHITE);
		// Background
		DrawTextureEx(game_midground, (Vector2){ scrolling_mid, 0 }, 0.0f, 2.0f, WHITE);
		DrawTextureEx(game_midground, (Vector2){ game_midground.width * 2 + scrolling_mid, 0 }, 0.0f, 2.0f, WHITE);
		// Foregorund
		DrawTextureEx(game_foreground, (Vector2){ scrolling_front, 0 }, 0.0f, 2.0f, WHITE);
		DrawTextureEx(game_foreground, (Vector2){ game_foreground.width * 2 + scrolling_front, 0 }, 0.0f, 2.0f, WHITE);

		
		// Text
		//DrawText("Hello Raylib", 200,200,20,WHITE);

		// --------------------------------------------------------------

		
		// end the frame and get ready for the next one  (display frame, poll input, etc...)
		EndDrawing();
	}

	// cleanup
	// unload our texture so it can be cleaned up
	UnloadTexture(game_foreground);
	UnloadTexture(game_midground);
	UnloadTexture(game_background);

	// destroy the window and cleanup the OpenGL context
	CloseWindow();
	return 0;
}
