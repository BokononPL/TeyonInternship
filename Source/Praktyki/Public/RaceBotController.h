// Copyright 2025 Teyon. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "RaceBotController.generated.h"

/**
 * 
 */
UCLASS()
class PRAKTYKI_API ARaceBotController : public AAIController
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite)
	FString BotName;

	UPROPERTY(BlueprintReadWrite)
	int32 CurrentLap;

	UPROPERTY(BlueprintReadWrite)
	int32 CurrentCheckpoint;

	UPROPERTY(BlueprintReadWrite)
	bool IsLapInvalidated;

	UPROPERTY(BlueprintReadWrite)
	TArray<float> LapTimes;
};
