// Copyright 2025 Teyon. All Rights Reserved.


#include "RaceState.h"

#include "HeadMountedDisplayTypes.h"
#include "PlayerScore.h"
#include "RacerController.h"
#include "RacerState.h"
#include "Net/UnrealNetwork.h"
#include "RacingGameMode.h"

ARaceState::ARaceState()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ARaceState::BeginPlay()
{
	Super::BeginPlay();
}

void ARaceState::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if(HasAuthority())
	{
		if(ARacingGameMode* CastGameMode = Cast<ARacingGameMode>(GetWorld()->GetAuthGameMode()))
		{
			TArray<FPlayerScore> PlayerScores;
			for(APlayerState* State : PlayerArray)
			{
				if(ARacerState* RacerState = Cast<ARacerState>(State))
				{
					if(APawn* Pawn = RacerState->GetPawn())
					{
						FVector RacerLocation = Pawn->GetActorLocation();
						FVector NextCheckpointLocation = CastGameMode->RaceCheckpoints[
							(RacerState->CurrentCheckpoint + 1) < CastGameMode->RaceCheckpoints.Num()
								? RacerState->CurrentCheckpoint + 1
								: 0]->GetActorLocation();
						float Distance = FVector::Dist(RacerLocation, NextCheckpointLocation);
						PlayerScores.Add(FPlayerScore(RacerState->GetUniqueId(), RacerState->CurrentLap,
						                              RacerState->CurrentCheckpoint, Distance));
					}
				}
			}
			PlayerScores.Sort();
			TArray<FUniqueNetIdRepl> tmp;
			for(FPlayerScore PlayerScore : PlayerScores)
			{
				tmp.Add(PlayerScore.PlayerNetId);
			}
			RacersOrder = tmp;
		}
	}
	if(IsCountdownStarted && GetServerWorldTimeSeconds() > RaceStartTime)
	{
		for(APlayerState* PlayerState : PlayerArray)
		{
			if(ARacerPawn* RacerPawn = Cast<ARacerPawn>(PlayerState->GetPawn()))
			{
				RacerPawn->SetDrivingEnabled(true);
			}
		}
		IsCountdownStarted = false;
	}
}

void ARaceState::HandleBeginPlay()
{
	Super::HandleBeginPlay();

	bReplicatedHasBegunPlay = true;

	GetWorldSettings()->NotifyBeginPlay();
	GetWorldSettings()->NotifyMatchStarted();
}

void ARaceState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ARaceState, RacersOrder);
	DOREPLIFETIME(ARaceState, MaxLaps);
	DOREPLIFETIME(ARaceState, MaxCheckpointIndex);
	DOREPLIFETIME(ARaceState, RaceStartTime);
	DOREPLIFETIME(ARaceState, IsCountdownStarted);
}

void ARaceState::StartCountdown()
{
	RaceStartTime = GetServerWorldTimeSeconds() + PreRaceTime + CountdownLength;
	IsCountdownStarted = true;
}
