// Copyright 2025 Teyon. All Rights Reserved.


#include "RaceState.h"

#include "MainGameInstance.h"
#include "PlayerScore.h"
#include "RaceBotController.h"
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
			TimeLimit = MainGameInstance->TimeLimit;
			ShouldInvalidateLaps = MainGameInstance->ShouldInvalidateLaps;
			GameType = MainGameInstance->GameType;
		}
	}
}

void ARaceState::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if(IsRaceOver) return;
	if(HasAuthority())
	{
		//add racer positions to scoreboard and sort it
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
						PlayerScores.Add(FPlayerScore(RacerState->RacerInfo.PlayerName, RacerState->CurrentLap, RacerState->IsLapInvalidated,
						                              RacerState->CurrentCheckpoint, Distance));
					}
				}
			}
			for(ARaceBotController* Bot : CastGameMode->Bots)
			{
				if(APawn* Pawn = Bot->GetPawn())
				{
					FVector RacerLocation = Pawn->GetActorLocation();
					FVector NextCheckpointLocation = CastGameMode->RaceCheckpoints[
						(Bot->CurrentCheckpoint + 1) < CastGameMode->RaceCheckpoints.Num()
							? Bot->CurrentCheckpoint + 1
							: 0]->GetActorLocation();
					float Distance = FVector::Dist(RacerLocation, NextCheckpointLocation);
					PlayerScores.Add(FPlayerScore(Bot->BotName, Bot->CurrentLap, Bot->IsLapInvalidated,
																  Bot->CurrentCheckpoint, Distance));
				}
			}
			PlayerScores.Sort();
			TArray<FString> tmp;
			for(FPlayerScore PlayerScore : PlayerScores)
			{
				tmp.Add(PlayerScore.Name);
			}
			RacersOrder = tmp;
		}
		if(GetServerWorldTimeSeconds() - RaceStartTime > TimeLimit * 60.0f)
		{
			// end race due to time limit
			IsRaceOver = true;
			for(APlayerState* State : PlayerArray)
			{
				if(ARacerState* RacerState = Cast<ARacerState>(State))
				{
					if(RacerState->CurrentLap < MaxLaps)
					{
						if(ARacerController* RacerController = Cast<ARacerController>(RacerState->GetPlayerController()))
						{
							if(ARacerPawn* RacerPawn = Cast<ARacerPawn>(RacerController->GetPawn()))
							{
								RacerPawn->AutoDrivingComponent->IsAutoDrivingEnabled = true;
							}
							FinishTimes.Add(FFinishInfo(RacerState->RacerInfo.PlayerName, -1.0f));
							RacerController->Client_OnRaceFinished(-1, RacerState->LapTimes);
						}
					}
				}
			}
			if(ARacingGameMode* CastGameMode = Cast<ARacingGameMode>(GetWorld()->GetAuthGameMode()))
			{
				for(ARaceBotController* Bot : CastGameMode->Bots)
				{
					if(Bot->CurrentLap < MaxLaps)
					{
						FinishTimes.Add(FFinishInfo(Bot->BotName, -1.0f));
					}
				}
			}
		}
	}
	if(IsCountdownStarted && GetServerWorldTimeSeconds() > RaceStartTime)
	{
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
				for(ARaceBotController* Bot : Gamemode->Bots)
				{
					Cast<ARacerPawn>(Bot->GetPawn())->SetDrivingEnabled(true);
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
	DOREPLIFETIME(ARaceState, IsRaceOver);
	DOREPLIFETIME(ARaceState, GameType);
}

void ARaceState::StartCountdown_Implementation()
{
	RaceStartTime = GetServerWorldTimeSeconds() + PreRaceTime + CountdownLength;
	IsCountdownStarted = true;
	
	for(APlayerState* State : PlayerArray)
	{
		if(ARacerState* RacerState = Cast<ARacerState>(State))
		{
			RacerState->CurrentLapStartTime = RaceStartTime;
		}
	}
}
