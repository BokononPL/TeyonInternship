// Copyright 2025 Teyon. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "FinishInfo.h"
#include "GameTypeEnum.h"
#include "Blueprint/UserWidget.h"
#include "GameOverWidget.generated.h"

/**
 * 
 */
UCLASS()
class PRAKTYKI_API UGameOverWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent)
	void SetValues(int32 Position, const TEnumAsByte<GameTypeEnum>& GameType);

	UFUNCTION(BlueprintImplementableEvent)
	void AddLapTime(int32 LapNumber, float LapTime);
};
