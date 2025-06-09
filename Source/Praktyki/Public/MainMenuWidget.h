// Copyright 2025 Teyon. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameTypeEnum.h"
#include "Blueprint/UserWidget.h"
#include "MainMenuWidget.generated.h"

/**
 * 
 */
UCLASS()
class PRAKTYKI_API UMainMenuWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent)
	void UpdateMaxLaps(int32 MaxLaps);

	UFUNCTION(BlueprintImplementableEvent)
	void UpdateTimeLimit(int32 TimeLimit);

	UFUNCTION(BlueprintImplementableEvent)
	void UpdateMaxPlayers(int32 MaxPlayers);

	UFUNCTION(BlueprintImplementableEvent)
	void UpdateShouldFillWithBots(bool ShouldFill);

	UFUNCTION(BlueprintImplementableEvent)
	void UpdateShouldInvalidateLaps(bool ShouldInvalidate);

	UFUNCTION(BlueprintImplementableEvent)
	void UpdatePlayerNames(const TArray<FString>& Names);

	UFUNCTION(BlueprintImplementableEvent)
	void UpdateGameType(const TEnumAsByte<GameTypeEnum>& GameType);
};
