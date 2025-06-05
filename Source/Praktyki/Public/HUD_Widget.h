// Copyright 2025 Teyon. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HUD_Widget.generated.h"

/**
 * 
 */
UCLASS()
class PRAKTYKI_API UHUD_Widget : public UUserWidget
{
	GENERATED_BODY()

public:
	void UpdateSpeed(float Value);

	UPROPERTY(BlueprintReadWrite)
	float Speed;

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void AddLapTime(int32 LapNumber, float LapTime);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void StartCountdown();
};
