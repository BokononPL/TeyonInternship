// Copyright 2025 Teyon. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "InputMappingContext.h"
#include "EnhancedInputSubsystems.h"
#include "HUD_Widget.h"
#include "RacerPawn.h"
#include "RacerController.generated.h"

/**
 * 
 */
UCLASS()
class PRAKTYKI_API ARacerController : public APlayerController
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	UInputMappingContext* MappingContext;

	UPROPERTY(BlueprintReadOnly, Category = "Interface")
	UHUD_Widget* HUD;

	UPROPERTY(BlueprintReadOnly, Category = "Interface")
	UUserWidget* GameOverWidget;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interface")
	TSubclassOf<UHUD_Widget> HUD_Class;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interface")
	TSubclassOf<UUserWidget> Menu_Class;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	ARacerPawn* PlayerVehiclePawn;

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void OnPossess(APawn* InPawn) override;
	virtual void SetupInputComponent() override;

public:
	UFUNCTION(BlueprintCallable)
	void OpenMenu();

	UFUNCTION(BlueprintCallable)
	void AddLapTime(int32 LapNumber, float LapTime);

	UFUNCTION(BlueprintCallable, Client, Reliable)
	void StartCountdown();

	UFUNCTION(BlueprintCallable, Client, Reliable)
	void SetPlayerInputEnabled(bool IsEnabled);

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void Server_NotifyIsReady();
	
	bool IsReady = false;
};
