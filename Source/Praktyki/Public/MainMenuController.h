// Copyright 2025 Teyon. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MainMenuWidget.h"
#include "GameFramework/PlayerController.h"
#include "MainMenuController.generated.h"

/**
 * 
 */
UCLASS()
class PRAKTYKI_API AMainMenuController : public APlayerController
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void ShowMainMenu();

	UFUNCTION(BlueprintCallable, Client, Reliable)
	void ClientShowMainMenu();

	UFUNCTION(BlueprintCallable, Client, Reliable)
	void ClientUpdateMaxLaps(int32 MaxLaps);

	UFUNCTION(BlueprintCallable, Client, Reliable)
	void ClientUpdateTimeLimit(int32 TimeLimit);

	UFUNCTION(BlueprintCallable, Client, Reliable)
	void ClientUpdateMaxPlayers(int32 MaxPlayers);

	UFUNCTION(BlueprintCallable, Client, Reliable)
	void ClientUpdateShouldFillWithBots(bool ShouldFill);

	UFUNCTION(BlueprintCallable, Client, Reliable)
	void ClientUpdateShouldInvalidateLaps(bool ShouldFill);

	UFUNCTION(BlueprintCallable, Client, Reliable)
	void ClientUpdatePlayerNames(const TArray<FString>& Names);

	UFUNCTION(BlueprintCallable, Client, Reliable)
	void ClientUpdateGameType(const GameTypeEnum& GameType);

	UFUNCTION(BlueprintCallable)
	void UpdateCarColor(bool ShouldUseDefaultPaintjob, FLinearColor Color);

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void Server_UpdateCarColor(bool ShouldUseDefaultPaintjob, FLinearColor Color);

	UFUNCTION(BlueprintCallable)
	void SetPlayerName(const FString& Name);

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void Server_SetPlayerName(const FString& Name);

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void Server_RequestMainMenuUpdate();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<UMainMenuWidget> MainMenuWidgetClass;

	UPROPERTY(BlueprintReadWrite)
	UMainMenuWidget* MainMenuWidget;
};
