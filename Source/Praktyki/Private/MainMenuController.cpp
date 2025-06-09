// Copyright 2025 Teyon. All Rights Reserved.


#include "MainMenuController.h"

#include "MainMenuGameMode.h"
#include "RacerState.h"
#include "Blueprint/UserWidget.h"

void AMainMenuController::ShowMainMenu()
{
	if(!MainMenuWidget)
	{
		if(MainMenuWidgetClass)
		{
			MainMenuWidget = Cast<UMainMenuWidget>(CreateWidget(this, MainMenuWidgetClass));
		}
	}
	MainMenuWidget->AddToViewport();
	SetShowMouseCursor(true);
	FInputModeUIOnly InputMode;
	SetInputMode(InputMode);
}

void AMainMenuController::ClientUpdateTimeLimit_Implementation(int32 TimeLimit)
{
	MainMenuWidget->UpdateTimeLimit(TimeLimit);
}

void AMainMenuController::ClientUpdateMaxLaps_Implementation(int32 MaxLaps)
{
	MainMenuWidget->UpdateMaxLaps(MaxLaps);
}

void AMainMenuController::ClientUpdateMaxPlayers_Implementation(int32 MaxPlayers)
{
	MainMenuWidget->UpdateMaxPlayers(MaxPlayers);
}

void AMainMenuController::ClientUpdateShouldFillWithBots_Implementation(bool ShouldFill)
{
	MainMenuWidget->UpdateShouldFillWithBots(ShouldFill);
}

void AMainMenuController::ClientUpdatePlayerNames_Implementation(const TArray<FString>& Names)
{
	MainMenuWidget->UpdatePlayerNames(Names);
}

void AMainMenuController::ClientUpdateShouldInvalidateLaps_Implementation(bool ShouldInvalidate)
{
	MainMenuWidget->UpdateShouldInvalidateLaps(ShouldInvalidate);
}

void AMainMenuController::ClientUpdateGameType_Implementation(const GameTypeEnum& GameType)
{
	MainMenuWidget->UpdateGameType(GameType);
}

void AMainMenuController::UpdateCarColor(bool ShouldUseDefaultPaintjob, FLinearColor Color)
{
	if(ARacerState* RacerState = GetPlayerState<ARacerState>())
	{
		RacerState->RacerInfo.ShouldUseDefaultPaintjob = ShouldUseDefaultPaintjob;
		RacerState->RacerInfo.CarColor = Color;
	}
	Server_UpdateCarColor(ShouldUseDefaultPaintjob, Color);
}

void AMainMenuController::Server_UpdateCarColor_Implementation(bool ShouldUseDefaultPaintjob, FLinearColor Color)
{
	if(ARacerState* RacerState = GetPlayerState<ARacerState>())
	{
		RacerState->RacerInfo.ShouldUseDefaultPaintjob = ShouldUseDefaultPaintjob;
		RacerState->RacerInfo.CarColor = Color;
	}
}

void AMainMenuController::SetPlayerName(const FString& Name)
{
	if(ARacerState* RacerState = GetPlayerState<ARacerState>())
	{
		RacerState->RacerInfo.PlayerName = Name;
	}
	Server_SetPlayerName(Name);
}

void AMainMenuController::Server_RequestMainMenuUpdate_Implementation()
{
	if(AMainMenuGameMode* MainMenuGameMode = Cast<AMainMenuGameMode>(GetWorld()->GetAuthGameMode()))
	{
		ClientUpdateMaxLaps(MainMenuGameMode->MaxLaps);
		ClientUpdateTimeLimit(MainMenuGameMode->TimeLimit);
		ClientUpdateMaxPlayers(MainMenuGameMode->MaxPlayersCount);
		ClientUpdateShouldFillWithBots(MainMenuGameMode->ShouldFillWithBots);
		ClientUpdateGameType(MainMenuGameMode->GameType);
		MainMenuGameMode->UpdatePlayerNames();
	}
}

void AMainMenuController::Server_SetPlayerName_Implementation(const FString& Name)
{
	if(ARacerState* RacerState = GetPlayerState<ARacerState>())
	{
		RacerState->RacerInfo.PlayerName = Name;
		if(AMainMenuGameMode* MainMenuGameMode = Cast<AMainMenuGameMode>(GetWorld()->GetAuthGameMode()))
		{
			MainMenuGameMode->UpdatePlayerNames();
		}
	}
}

void AMainMenuController::ClientShowMainMenu_Implementation()
{
	ShowMainMenu();
}
