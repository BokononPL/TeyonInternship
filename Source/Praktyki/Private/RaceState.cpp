// Copyright 2025 Teyon. All Rights Reserved.


#include "RaceState.h"

#include "MainGameInstance.h"
#include "PlayerScore.h"
#include "RacerController.h"
#include "RacerState.h"
#include "Net/UnrealNetwork.h"
#include "RacingGameMode.h"
#include "Kismet/KismetStringLibrary.h"

ARaceState::ARaceState()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ARaceState::BeginPlay()
{
	Super::BeginPlay();

	if(HasAuthority())
	{
		if(UMainGameInstance* MainGameInstance = Cast<UMainGameInstance>(GetGameInstance()))
		{
			MaxLaps = MainGameInstance->MaxLaps;
		}
	}
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
						PlayerScores.Add(FPlayerScore(RacerState->GetUniqueId(), RacerState->CurrentLap, RacerState->IsLapInvalidated,
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
	Print("COUNTDOWN STARTED: " + UKismetStringLibrary::Conv_BoolToString(IsCountdownStarted), 0.0f, FColor::Red);
	if(IsCountdownStarted && GetServerWorldTimeSeconds() > RaceStartTime)
	{
		Print("RACE START", 10.0f, FColor::Red);
		for(APlayerState* PlayerState : PlayerArray)
		{
			ARacerPawn* RacerPawn = Cast<ARacerPawn>(PlayerState->GetPawn());
			RacerPawn->SetDrivingEnabled(true);
		}
		IsCountdownStarted = false;

		if(HasAuthority())
		{
			if(ARacingGameMode* Gamemode = Cast<ARacingGameMode>(GetWorld()->GetAuthGameMode()))
			{
				if(Gamemode->Bot)
				{
					Gamemode->Bot->SetDrivingEnabled(true);
				}
			}
		}
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
	DOREPLIFETIME(ARaceState, FinishTimes);
}

void ARaceState::StartCountdown_Implementation()
{
	RaceStartTime = GetServerWorldTimeSeconds() + PreRaceTime + CountdownLength;
	IsCountdownStarted = true;
}
