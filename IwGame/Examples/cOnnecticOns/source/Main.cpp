/*

  Copyright 2012 Pocketeers Limited.  All rights reserved.

  These coded instructions, statements, and computer programs contain proprietary information of Pocketeers Limited 
  that is used in an actual commercial product known as cOnnecticOns. This source code has been provided as a working 
  example of the IwGame Engine. You are hereby granted permission to use this code to learn to create your own products
  using the IwGame Engnie as long as the product(s) in question vary significantly in design to cOnnecticOns.

*/

// Marmalade headers
#include "s3e.h"

// IwGame engine headers

// Game headers
#include "Common.h"
#include "Game.h"
#include "Marm\IwGamePlatformMarm.h"

int main()
{
	// Init Marmalade platform (do not allocate or try to use Marmlade before this point)
	IwGamePlatformMarm_Create();

	CIwGameError::LogError("BOOT: Memory used: ", CIwGameString(s3eMemoryGetInt(S3E_MEMORY_USED)).c_str());
	CIwGameError::LogError("BOOT: Memory free: ", CIwGameString(s3eMemoryGetInt(S3E_MEMORY_FREE)).c_str());

	// Init Game
	Game::Create();
	GAME->Init(true);

	CIwGameError::LogError("POST_INIT: Memory used: ", CIwGameString(s3eMemoryGetInt(S3E_MEMORY_USED)).c_str());
	CIwGameError::LogError("POST_INIT: Memory free: ", CIwGameString(s3eMemoryGetInt(S3E_MEMORY_FREE)).c_str());

	// Start the compass
	IW_GAME_INPUT->startCompass();

	// Main Game Loop
	while (!s3eDeviceCheckQuitRequest())
	{
		// Update the game
		if (!GAME->Update())
			break;

		// Draw the scene
		GAME->Draw();
	}

	GAME->Release();
	Game::Destroy();

	CIwGameError::LogError("EXIT: Memory used: ", CIwGameString(s3eMemoryGetInt(S3E_MEMORY_USED)).c_str());
	CIwGameError::LogError("EXIT: Memory free: ", CIwGameString(s3eMemoryGetInt(S3E_MEMORY_FREE)).c_str());

	// Shut down Marmalade platform (do not allocate / free memory or call marmalade beyond this point)
	IwGamePlatformMarm_Destroy();

    return 0;
}

