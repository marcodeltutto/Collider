// Marmalade headers
#include "s3e.h"

// IwGame engine headers

// Game headers
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

		// Check for back button quit
		if (IW_GAME_INPUT->isKeyDown(s3eKeyAbsBSK))
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

