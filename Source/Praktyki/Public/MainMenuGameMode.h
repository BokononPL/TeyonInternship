// Copyright 2025 Teyon. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MainMenuController.h"
#include "GameFramework/GameModeBase.h"
#include "CPPUtils.h"
#include "MainMenuGameMode.generated.h"

/**
 * 
 */
UCLASS()
class PRAKTYKI_API AMainMenuGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	virtual void BeginPlay() override;
	virtual void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;

	UPROPERTY(BlueprintReadWrite)
	TArray<AMainMenuController*> MainMenuControllers;

	UPROPERTY(BlueprintReadWrite)
	int32 MaxLaps;
};
