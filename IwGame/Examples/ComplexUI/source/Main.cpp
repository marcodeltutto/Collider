// Marmalade headers
#include "s3e.h"

// IwGame engine headers
#include "Marm\IwGamePlatformMarm.h"

// Game headers
#include "Game.h"

int main()
{
	// Init Marmalade platform (do not allocate or try to use Marmlade before this point)
	IwGamePlatformMarm_Create();

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

	// Shut down Marmalade platform (do not allocate / free memory or call marmalade beyond this point)
	IwGamePlatformMarm_Destroy();

    return 0;
}

