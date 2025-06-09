#pragma once
#include "PlayerScore.generated.h"

USTRUCT(BlueprintType)
struct PRAKTYKI_API FPlayerScore
{
	GENERATED_BODY()

public:
	FPlayerScore(){};
	
	FPlayerScore(FString Name, int32 LapNumber, bool IsLapInvalidated, int32 CheckpointNumber, float DistanceToNextCheckpoint)
	{
		this->Name = Name;
		this->LapNumber = LapNumber;
		this->IsLapInvalidated = IsLapInvalidated;
		this->CheckpointNumber = CheckpointNumber;
		this->DistanceToNextCheckpoint = DistanceToNextCheckpoint;
	}

	bool operator<(const FPlayerScore& Other) const
	{
		int LapDifference = LapNumber -(IsLapInvalidated) - Other.LapNumber + (Other.IsLapInvalidated);
		if(LapDifference != 0) return LapDifference > 0;
		if(CheckpointNumber != Other.CheckpointNumber) return CheckpointNumber > Other.CheckpointNumber;
		return DistanceToNextCheckpoint < Other.DistanceToNextCheckpoint;
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Name;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 LapNumber;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsLapInvalidated;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 CheckpointNumber;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DistanceToNextCheckpoint;
};