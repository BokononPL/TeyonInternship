// Copyright 2025 Teyon. All Rights Reserved.


#include "RacerController.h"

#include "RacerState.h"
#include "RaceState.h"
#include "RacingGameMode.h"
#include "Blueprint/WidgetLayoutLibrary.h"

void ARacerController::BeginPlay()
{
	Super::BeginPlay();
	if(IsLocalPlayerController())
	{
		UWidgetLayoutLibrary::RemoveAllWidgets(GetWorld());
		HUD = CreateWidget<UHUD_Widget>(this, HUD_Class);
		HUD->AddToViewport();
	}
	FInputModeGameOnly InputMode;
	SetInputMode(InputMode);
}

void ARacerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if(IsLocalPlayerController())
	{
		if(!PlayerVehiclePawn)
		{
			PlayerVehiclePawn = Cast<ARacerPawn>(GetPawn());
			return;
		}
		HUD->UpdateSpeed(FMath::Abs(PlayerVehiclePawn->VehicleMovementComponent->GetForwardSpeed()));
		if(!IsReady)
		{
			IsReady = true;
			Server_NotifyIsReady();
		}
	}
}

void ARacerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
}

void ARacerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	if(UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(DefaultMappingContext, 0);
		Subsystem->AddMappingContext(SteeringWheelMappingContext, 1);
	}
}

void ARacerController::OpenMenu()
{
	UUserWidget* Menu = CreateWidget(this, Menu_Class);
	Menu->AddToViewport();
	FInputModeUIOnly InputMode;
	SetInputMode(InputMode);
	SetShowMouseCursor(true);
}

void ARacerController::AddLapTime_Implementation(int32 LapNumber, float LapTime)
{
	if(IsLocalPlayerController() && HUD)
	{
		HUD->AddLapTime(LapNumber, LapTime);
	}
}

void ARacerController::Client_OnRaceFinished_Implementation(int32 Position, const TArray<float>& LapTimes)
{
	UWidgetLayoutLibrary::RemoveAllWidgets(GetWorld());
	GameOverWidget = CreateWidget<UGameOverWidget>(this, GameOverWidget_Class);
	GameOverWidget->AddToViewport();
	if(ARaceState* RaceState = GetWorld()->GetGameState<ARaceState>())
	{
		GameOverWidget->SetValues(Position, RaceState->GameType);
		for(int i = 0; i < LapTimes.Num(); i++)
		{
			GameOverWidget->AddLapTime(i + 1, LapTimes[i]);
		}
	}
	FInputModeUIOnly InputMode;
	FlushPressedKeys();
	SetInputMode(InputMode);
	SetShowMouseCursor(true);
}

void ARacerController::Server_NotifyIsReady_Implementation()
{
	IsReady = true;
	if(ARacingGameMode* RacingGameMode = Cast<ARacingGameMode>(GetWorld()->GetAuthGameMode()))
	{
		RacingGameMode->OnPlayerBecomesReady();
	}
}
