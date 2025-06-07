#pragma once
#include "FinishInfo.generated.h"

USTRUCT(BlueprintType)
struct PRAKTYKI_API FFinishInfo
{
	GENERATED_BODY()

public:
	FFinishInfo(){};
	FFinishInfo(FUniqueNetIdRepl PlayerNetId, float FinishTime)
	{
		this->PlayerNetId = PlayerNetId;
		this->FinishTime = FinishTime;
	};
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FUniqueNetIdRepl PlayerNetId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float FinishTime;
};
