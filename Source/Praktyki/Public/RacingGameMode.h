// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RaceCheckpoint.h"
#include "GameFramework/GameModeBase.h"
#include "FinishInfo.h"
#include "RacerPawn.h"
#include "RacingGameMode.generated.h"

/**
 * 
 */
UCLASS()
class PRAKTYKI_API ARacingGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ARacingGameMode();
	virtual void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;
	virtual void PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage) override;

	void OnPlayerBecomesReady();

	UFUNCTION(BlueprintCallable)
	void RestartRace();

	int32 ExpectedPlayersCount;
	TArray<class ARacerController*> PlayerControllers;
	TArray<ARaceCheckpoint*> RaceCheckpoints;
	TArray<class ARaceBotController*> Bots; 

private:
	bool HasFirstPlayerConnected = false;
	float FirstPlayerConnectedTime = 0.0f;
	bool HasInitializedStartsArray = false;
	TArray<AActor*> StartsArray;
	int32 UsedStartsCount = 0;
	TArray<FString> BotNames = {"BotAlbert", "BotBetty", "BotCindy", "BotDamian", "BotEddy", "BotFrancis", "BotGerard", "BotHarold"};
};
