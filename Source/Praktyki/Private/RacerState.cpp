// Copyright 2025 Teyon. All Rights Reserved.


#include "RacerState.h"

#include "CPPUtils.h"
#include "Net/UnrealNetwork.h"

ARacerState::ARacerState()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ARacerState::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ARacerState, CurrentLap);
	DOREPLIFETIME(ARacerState, CurrentCheckpoint);
	DOREPLIFETIME(ARacerState, RacerInfo)
}

void ARacerState::CopyProperties(APlayerState* PlayerState)
{
	Super::CopyProperties(PlayerState);
	ARacerState* NewRacerState = Cast<ARacerState>(PlayerState);
	NewRacerState->RacerInfo = RacerInfo;
}

void ARacerState::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(GetPawn())
	{
		Print("Pawn: " + GetPawn()->GetName(), 0.0f);
	}
	else
	{
		Print("No Pawn", 0.0f);
	}
}
