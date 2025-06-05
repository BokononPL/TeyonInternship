// Fill out your copyright notice in the Description page of Project Settings.


#include "RacingGameMode.h"

#include <string>

#include "MainGameInstance.h"
#include "RacerController.h"
#include "RacerState.h"
#include "RaceState.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"

ARacingGameMode::ARacingGameMode()
{
	PrimaryActorTick.bCanEverTick = true;
	DefaultPawnClass = nullptr;
}

void ARacingGameMode::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
{
	Super::HandleStartingNewPlayer_Implementation(NewPlayer);

	PlayerControllers.Add(Cast<ARacerController>(NewPlayer));

	HasFirstPlayerConnected = true;
	
	if(PlayerControllers.Num() == ExpectedPlayersCount) //all expected players have connected
	{
		for(int i = 0; i < PlayerControllers.Num(); i++)
		{
			RestartPlayerAtPlayerStart(PlayerControllers[i], ChoosePlayerStart(PlayerControllers[i]));
		}
	}
}

void ARacingGameMode::BeginPlay()
{
	Super::BeginPlay();

	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ARaceCheckpoint::StaticClass(), FoundActors);

	for (AActor* Actor : FoundActors)
	{
		if (ARaceCheckpoint* CastActor = Cast<ARaceCheckpoint>(Actor))
		{
			RaceCheckpoints.Add(CastActor);
		}
	}
	RaceCheckpoints.Sort();
	if(ARaceState* CastState = Cast<ARaceState>(GameState))
	{
		CastState->MaxCheckpointIndex = RaceCheckpoints.Num() - 1;
	}

	if(UMainGameInstance* GameInstance = Cast<UMainGameInstance>(GetWorld()->GetGameInstance()))
	{
		MaxLaps = GameInstance->MaxLaps;
		ExpectedPlayersCount = GameInstance->ConnectedPlayersCount;
	}
}

void ARacingGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

APawn* ARacingGameMode::SpawnDefaultPawnFor_Implementation(AController* NewPlayer, AActor* StartSpot)
{
	//return Super::SpawnDefaultPawnFor_Implementation(NewPlayer, StartSpot);

	FActorSpawnParameters SpawnParameters;
	SpawnParameters.Owner = NewPlayer;
	APawn* NewPawn = GetWorld()->SpawnActor<APawn>(PawnClass, StartSpot->GetActorLocation(), StartSpot->GetActorRotation(), SpawnParameters);
	if(NewPawn)
	{
		NewPlayer->Possess(NewPawn);
		if(ARacerPawn* RacerPawn = Cast<ARacerPawn>(NewPawn))
		{
			RacerPawn->SetDrivingEnabled(false);
		}
	}
	return NewPawn;
}

AActor* ARacingGameMode::ChoosePlayerStart_Implementation(AController* Player)
{
	//return Super::ChoosePlayerStart_Implementation(Player);
	if(!HasInitializedStartsArray)
	{
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), StartsArray);
		HasInitializedStartsArray = true;
	}
	AActor* OutStart = nullptr;
	if(UsedStartsCount < StartsArray.Num())
	{
		OutStart = StartsArray[UsedStartsCount];
		UsedStartsCount++;
	}
	else
	{
		Print("Not enough start spots!", 15.0f, FColor::Red);
	}
	return OutStart;
}

void ARacingGameMode::OnPlayerBecomesReady()
{
	for(ARacerController* Controller : PlayerControllers)
	{
		if(!Controller->IsReady) return;
	}
	if(ARaceState* RaceState = Cast<ARaceState>(GameState))
	{
		RaceState->StartCountdown();
	}
}
