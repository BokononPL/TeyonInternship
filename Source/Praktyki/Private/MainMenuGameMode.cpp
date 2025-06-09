// Copyright 2025 Teyon. All Rights Reserved.


#include "MainMenuGameMode.h"

#include "AudioMixerDevice.h"
#include "RacerState.h"

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
}

void AMainMenuGameMode::PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId,
	FString& ErrorMessage)
{
	Super::PreLogin(Options, Address, UniqueId, ErrorMessage);

	if(MainMenuControllers.Num() >= MaxPlayersCount)
	{
		ErrorMessage = "The lobby is full!";
	}
}

void AMainMenuGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);
	if(AMainMenuController* MainMenuController = Cast<AMainMenuController>(Exiting))
	{
		MainMenuControllers.Remove(MainMenuController);
	}
	UpdatePlayerNames();
}

void AMainMenuGameMode::UpdatePlayerNames()
{
	TArray<FString> PlayerNames = GetPlayerNames();
	for(AMainMenuController* MainMenuController : MainMenuControllers)
	{
		MainMenuController->ClientUpdatePlayerNames(PlayerNames);
	}
}

TArray<FString> AMainMenuGameMode::GetPlayerNames()
{
	TArray<FString> PlayerNames;
	for(AMainMenuController* MainMenuController : MainMenuControllers)
	{
		if(ARacerState* RacerState = MainMenuController->GetPlayerState<ARacerState>())
		{
			PlayerNames.Add(RacerState->RacerInfo.PlayerName);
		}
	}
	return PlayerNames;
}

void AMainMenuGameMode::SetMaxLaps(int32 NewMaxLaps)
{
	MaxLaps = NewMaxLaps;
	for(AMainMenuController* MainMenuController : MainMenuControllers)
	{
		MainMenuController->ClientUpdateMaxLaps(NewMaxLaps);
	}
}

void AMainMenuGameMode::SetTimeLimit(int32 NewTimeLimit)
{
	TimeLimit = NewTimeLimit;
	for(AMainMenuController* MainMenuController : MainMenuControllers)
	{
		MainMenuController->ClientUpdateTimeLimit(NewTimeLimit);
	}
}

void AMainMenuGameMode::SetMaxPlayersCount(int32 NewMaxPlayersCount)
{
	MaxPlayersCount = NewMaxPlayersCount;
	for(AMainMenuController* MainMenuController : MainMenuControllers)
	{
		MainMenuController->ClientUpdateMaxPlayers(NewMaxPlayersCount);
	}
}

void AMainMenuGameMode::SetShouldFillWithBots(bool NewShouldFillWithBots)
{
	ShouldFillWithBots = NewShouldFillWithBots;
	for(AMainMenuController* MainMenuController : MainMenuControllers)
	{
		MainMenuController->ClientUpdateShouldFillWithBots(NewShouldFillWithBots);
	}
}

void AMainMenuGameMode::SetShouldInvalidateLaps(bool NewShouldInvalidateLaps)
{
	ShouldInvalidateLaps = NewShouldInvalidateLaps;
	for(AMainMenuController* MainMenuController : MainMenuControllers)
	{
		MainMenuController->ClientUpdateShouldInvalidateLaps(NewShouldInvalidateLaps);
	}
}

void AMainMenuGameMode::SetGameType(const GameTypeEnum& NewGameType)
{
	GameType = NewGameType;
	for(AMainMenuController* MainMenuController : MainMenuControllers)
	{
		MainMenuController->ClientUpdateGameType(NewGameType);
	}
}
