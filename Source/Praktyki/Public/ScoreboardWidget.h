// Copyright 2025 Teyon. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ScoreboardWidget.generated.h"

/**
 * 
 */

UCLASS()
class PRAKTYKI_API UScoreboardWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
};
