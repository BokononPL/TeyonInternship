#pragma once
#include "FinishInfo.generated.h"

USTRUCT(BlueprintType)
struct PRAKTYKI_API FFinishInfo
{
	GENERATED_BODY()

public:
	FFinishInfo(){};
	FFinishInfo(FString Name, float FinishTime)
	{
		this->Name = Name;
		this->FinishTime = FinishTime;
	};
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float FinishTime;
};
