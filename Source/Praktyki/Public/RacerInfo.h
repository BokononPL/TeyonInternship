#pragma once
#include "RacerInfo.generated.h"

USTRUCT(BlueprintType)
struct PRAKTYKI_API FRacerInfo
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString PlayerName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FLinearColor CarColor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool ShouldUseDefaultPaintjob;
};