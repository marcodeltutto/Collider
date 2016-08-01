// Marmalade headers
#include "s3e.h"
#include "Iw2D.h"
#include <iostream>


// IwGame engine headers
#include "CzPlatform.h"

// Game headers
#include "Game.h"


int main()
{
    
	//marco CIwGameError::LogError("BOOT: Memory used: ", CIwGameString(s3eMemoryGetInt(S3E_MEMORY_USED)).c_str());
  std::cout <<"BOOT: Memory used: "<<CzString(s3eMemoryGetInt(S3E_MEMORY_USED)).c_str() << std::endl;
	//marco CIwGameError::LogError("BOOT: Memory free: ", CIwGameString(s3eMemoryGetInt(S3E_MEMORY_FREE)).c_str());
  std::cout <<"BOOT: Memory free: "<< CzString(s3eMemoryGetInt(S3E_MEMORY_FREE)).c_str() << std::endl;
  

	// Init Game
    CzPlatform_Create();
  
	Game::Create();

	GAME->Init(true);

	//marco CIwGameError::LogError("POST_INIT: Memory used: ", CIwGameString(s3eMemoryGetInt(S3E_MEMORY_USED)).c_str());
  std::cout <<"BOOT2: Memory used: "<<CzString(s3eMemoryGetInt(S3E_MEMORY_USED)).c_str() << std::endl;
  	//marco CIwGameError::LogError("POST_INIT: Memory free: ", CIwGameString(s3eMemoryGetInt(S3E_MEMORY_FREE)).c_str());
  std::cout <<"BOOT2: Memory free: "<< CzString(s3eMemoryGetInt(S3E_MEMORY_FREE)).c_str() << std::endl;

    int memUsed = s3eMemoryGetInt(S3E_MEMORY_USED);
    
	// Start the compass
	//IW_GAME_INPUT->startCompass();

	// Main Game Loop
	while (!PLATFORM_SYS->CheckAppQuit())//!s3eDeviceCheckQuitRequest())
	{
		
		// Update the game
		if (!GAME->Update()){
			std::cout <<" !!! GAME->Update()== NULL !!!\n";
			break;
		}

		// Check for back button quit
		if (CZ_INPUT->isKeyDown(s3eKeyAbsBSK)){
			std::cout <<" !!! Back Key is Down !!!\n";
			break;
		}
		// Draw the scene
		GAME->Draw();
      
//        CIwGameError::LogError("RUNNING: Memory used: ", CIwGameString(s3eMemoryGetInt(S3E_MEMORY_USED)).c_str());
//        CIwGameError::LogError("RUNNING: Memory free: ", CIwGameString(s3eMemoryGetInt(S3E_MEMORY_FREE)).c_str());
        
        if(s3eMemoryGetInt(S3E_MEMORY_USED) > memUsed)
            memUsed = s3eMemoryGetInt(S3E_MEMORY_USED);
	}
    //marco CIwGameError::LogError("TOTAL: Memory used: ", CIwGameString(memUsed).c_str());
	
	
	GAME->Release();
	Game::Destroy();
  
    CzPlatform_Destroy();   // Shut down platform

	//marco CIwGameError::LogError("EXIT: Memory used: ", CIwGameString(s3eMemoryGetInt(S3E_MEMORY_USED)).c_str());
	//marco CIwGameError::LogError("EXIT: Memory free: ", CIwGameString(s3eMemoryGetInt(S3E_MEMORY_FREE)).c_str());

    return 0;
}

