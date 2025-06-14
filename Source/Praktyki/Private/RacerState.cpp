// Copyright 2025 Teyon. All Rights Reserved.


#include "RacerState.h"

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
	DOREPLIFETIME(ARacerState, IsLapInvalidated);
	DOREPLIFETIME(ARacerState, LapTimes);
	DOREPLIFETIME(ARacerState, BestLapTime);
	DOREPLIFETIME(ARacerState, CurrentLapStartTime)
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
}

void ARacerState::SetCurrentLap(int32 LapIndex)
{
	CurrentLap = LapIndex;
	Server_SetCurrentLap(LapIndex);
}

void ARacerState::Server_SetCurrentLap_Implementation(int32 LapIndex)
{
	CurrentLap = LapIndex;
}

void ARacerState::SetCurrentCheckpoint(int32 CheckpointIndex)
{
	CurrentCheckpoint = CheckpointIndex;
	Server_SetCurrentCheckpoint(CheckpointIndex);
}

void ARacerState::Server_SetCurrentCheckpoint_Implementation(int32 CheckpointIndex)
{
	CurrentCheckpoint = CheckpointIndex;
}
