// Copyright 2025 Teyon. All Rights Reserved.


#include "RacerController.h"

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
		Subsystem->AddMappingContext(MappingContext, 0);
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

void ARacerController::AddLapTime(int32 LapNumber, float LapTime)
{
	if(IsLocalController() && HUD)
	{
		HUD->AddLapTime(LapNumber, LapTime);
	}
}

void ARacerController::Server_NotifyIsReady_Implementation()
{
	IsReady = true;
	if(ARacingGameMode* RacingGameMode = Cast<ARacingGameMode>(GetWorld()->GetAuthGameMode()))
	{
		RacingGameMode->OnPlayerBecomesReady();
	}
}

void ARacerController::SetPlayerInputEnabled_Implementation(bool IsEnabled)
{
	Print("Controller says: input enabled called", 15.0f);
	if(!GetPawn()) return;
	if(IsEnabled)
	{
		GetPawn()->EnableInput(this);
	}
	else
	{
		GetPawn()->DisableInput(this);
	}
}

void ARacerController::StartCountdown_Implementation()
{
	if(HUD)
	{
		HUD->StartCountdown();
		Print("Controller says: start countdown", 15.0f);
	}
}
