// Copyright 2025 Teyon. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "FinishInfo.h"
#include "GameTypeEnum.h"
#include "RaceState.generated.h"

/**
 * 
 */
UCLASS()
class PRAKTYKI_API ARaceState : public AGameStateBase
{
	GENERATED_BODY()

public:
	ARaceState();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void HandleBeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(NetMulticast, Reliable)
	void StartCountdown();

	UPROPERTY(Replicated, BlueprintReadWrite)
	TArray<FString> RacersOrder;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	int32 MaxLaps = 3;

	//Currently only tarmac, but more would be supported
	UPROPERTY(EditDefaultsOnly)
	TArray<UPhysicalMaterial*> OnTrackMaterials;

	//Currently only grass, but more would be supported
	UPROPERTY(EditDefaultsOnly)
	TArray<UPhysicalMaterial*> OffTrackMaterials;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	int32 MaxCheckpointIndex;

	UPROPERTY(BlueprintReadOnly, Replicated)
	float RaceStartTime;

	UPROPERTY(BlueprintReadOnly)
	float PreRaceTime = 3.0f;

	UPROPERTY(BlueprintReadOnly)
	float CountdownLength = 3.0f;

	UPROPERTY(BlueprintReadOnly)
	bool IsCountdownStarted = false;

	UPROPERTY(BlueprintReadOnly)
	bool ShouldInvalidateLaps = false;

	UPROPERTY(BlueprintReadOnly, Replicated)
	TArray<FFinishInfo> FinishTimes;

	UPROPERTY(BlueprintReadOnly, Replicated)
	int32 TimeLimit;

	UPROPERTY(BlueprintReadOnly, Replicated)
	TEnumAsByte<GameTypeEnum> GameType;

	UPROPERTY(BlueprintReadOnly, Replicated)
	bool IsRaceOver = false;
};
