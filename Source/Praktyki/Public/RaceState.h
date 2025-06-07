// Copyright 2025 Teyon. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "CPPUtils.h"
#include "FinishInfo.h"
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
	TArray<FUniqueNetIdRepl> RacersOrder;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	int32 MaxLaps = 3;

	UPROPERTY(EditDefaultsOnly)
	UPhysicalMaterial* OffTrackMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	int32 MaxCheckpointIndex;

	UPROPERTY(BlueprintReadOnly, Replicated)
	float RaceStartTime;

	UPROPERTY(BlueprintReadOnly)
	float PreRaceTime = 5.0f;

	UPROPERTY(BlueprintReadOnly)
	float CountdownLength = 3.0f;

	UPROPERTY(BlueprintReadOnly)
	bool IsCountdownStarted = false;

	UPROPERTY(BlueprintReadOnly, Replicated)
	TArray<FFinishInfo> FinishTimes;
};
