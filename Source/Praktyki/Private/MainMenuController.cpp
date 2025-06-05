// Copyright 2025 Teyon. All Rights Reserved.


#include "MainMenuController.h"

#include "RacerState.h"
#include "Blueprint/UserWidget.h"

void AMainMenuController::ShowMainMenu()
{
	if(!MainMenuWidget)
	{
		if(MainMenuWidgetClass)
		{
			MainMenuWidget = CreateWidget(this, MainMenuWidgetClass);
		}
	}
	MainMenuWidget->AddToViewport();
	SetShowMouseCursor(true);
	FInputModeUIOnly InputMode;
	SetInputMode(InputMode);
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

void AMainMenuController::UpdatePlayerName(const FString& Name)
{
	if(ARacerState* RacerState = GetPlayerState<ARacerState>())
	{
		RacerState->RacerInfo.PlayerName = Name;
	}
	Server_UpdatePlayerName(Name);
}

void AMainMenuController::Server_UpdatePlayerName_Implementation(const FString& Name)
{
	if(ARacerState* RacerState = GetPlayerState<ARacerState>())
	{
		RacerState->RacerInfo.PlayerName = Name;
	}
}

void AMainMenuController::ClientShowMainMenu_Implementation()
{
	ShowMainMenu();
}
