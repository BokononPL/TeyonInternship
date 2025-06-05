// Copyright 2025 Teyon. All Rights Reserved.


#include "MainMenuGameMode.h"

void AMainMenuGameMode::BeginPlay()
{
	Super::BeginPlay();
}

void AMainMenuGameMode::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
{
	Super::HandleStartingNewPlayer_Implementation(NewPlayer);

	if(AMainMenuController* MainMenuController = Cast<AMainMenuController>(NewPlayer))
	{
		MainMenuControllers.Add(MainMenuController);
		MainMenuController->ClientShowMainMenu();
	}
	Print("New Player Joined", 15.0f);
}
