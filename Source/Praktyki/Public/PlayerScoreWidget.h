// Copyright 2025 Teyon. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerScoreWidget.generated.h"

/**
 * 
 */

UCLASS()
class PRAKTYKI_API UPlayerScoreWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void UpdatePlayerScore(int32 Position);
};
