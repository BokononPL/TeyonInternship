#pragma once
#include "PlayerScore.generated.h"

USTRUCT(BlueprintType)
struct PRAKTYKI_API FPlayerScore
{
	GENERATED_BODY()

public:
	FPlayerScore(){};
	
	FPlayerScore(FUniqueNetIdRepl PlayerNetId, int32 LapNumber, int32 CheckpointNumber, float DistanceToNextCheckpoint)
	{
		this->PlayerNetId = PlayerNetId;
		this->LapNumber = LapNumber;
		this->CheckpointNumber = CheckpointNumber;
		this->DistanceToNextCheckpoint = DistanceToNextCheckpoint;
	}

	bool operator<(const FPlayerScore& Other) const
	{
		if(LapNumber != Other.LapNumber) return LapNumber > Other.LapNumber;
		if(CheckpointNumber != Other.CheckpointNumber) return CheckpointNumber > Other.CheckpointNumber;
		return DistanceToNextCheckpoint < Other.DistanceToNextCheckpoint;
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FUniqueNetIdRepl PlayerNetId;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 LapNumber;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 CheckpointNumber;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DistanceToNextCheckpoint;
};