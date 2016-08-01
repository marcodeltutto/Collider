// Marmalade headers
#include "s3e.h"

// IwGame engine headers

// Game headers
#include "Common.h"
#include "Game.h"

int main()
{
	CIwGameError::LogError("BOOT: Memory used: ", CIwGameString(s3eMemoryGetInt(S3E_MEMORY_USED)).c_str());
	CIwGameError::LogError("BOOT: Memory free: ", CIwGameString(s3eMemoryGetInt(S3E_MEMORY_FREE)).c_str());

	// Init Game
	Game::Create();
	GAME->Init(true);

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

    return 0;
}

