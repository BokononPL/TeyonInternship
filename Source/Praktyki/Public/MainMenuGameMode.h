// Copyright 2025 Teyon. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MainMenuController.h"
#include "GameFramework/GameModeBase.h"
#include "CPPUtils.h"
#include "MainMenuGameMode.generated.h"

/**
 * 
 */
UCLASS()
class PRAKTYKI_API AMainMenuGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	virtual void BeginPlay() override;
	virtual void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;

	UPROPERTY(BlueprintReadWrite)
	TArray<AMainMenuController*> MainMenuControllers;

	UPROPERTY(BlueprintReadWrite, Setter = SetMaxLaps)
	int32 MaxLaps;

	UPROPERTY(BlueprintReadWrite, Setter = SetTimeLimit)
	int32 TimeLimit;

	UPROPERTY(BlueprintReadWrite, Setter = SetMaxPlayersCount)
	int32 MaxPlayersCount;

	UPROPERTY(BlueprintReadWrite, Setter = SetShouldFillWithBots)
	bool ShouldFillWithBots;

	UFUNCTION(BlueprintCallable)
	void UpdatePlayerNames();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	TArray<FString> GetPlayerNames();

	void SetMaxLaps(int32 NewMaxLaps);
	void SetTimeLimit(int32 NewTimeLimit);
	void SetMaxPlayersCount(int32 NewMaxPlayersCount);
	void SetShouldFillWithBots(bool NewShouldFillWithBots);
};
