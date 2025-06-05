// Copyright 2025 Teyon. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
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

	UFUNCTION(BlueprintCallable)
	void UpdateCarColor(bool ShouldUseDefaultPaintjob, FLinearColor Color);

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void Server_UpdateCarColor(bool ShouldUseDefaultPaintjob, FLinearColor Color);

	UFUNCTION(BlueprintCallable)
	void UpdatePlayerName(const FString& Name);

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void Server_UpdatePlayerName(const FString& Name);

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<UUserWidget> MainMenuWidgetClass;

	UPROPERTY(BlueprintReadWrite)
	UUserWidget* MainMenuWidget;
};
