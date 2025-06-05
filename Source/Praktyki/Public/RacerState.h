// Copyright 2025 Teyon. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "RacerInfo.h"
#include "GameFramework/PlayerState.h"
#include "RacerState.generated.h"

/**
 * 
 */
UCLASS()
class PRAKTYKI_API ARacerState : public APlayerState
{
	GENERATED_BODY()

public:
	ARacerState();
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void CopyProperties(APlayerState* PlayerState) override;
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	int32 CurrentLap = 0;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	int32 CurrentCheckpoint = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	FRacerInfo RacerInfo;

	UPROPERTY(BlueprintReadOnly, Replicated)
	TArray<float> LapTimes;
};
