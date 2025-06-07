// Fill out your copyright notice in the Description page of Project Settings.


#include "RacingGameMode.h"

#include <string>

#include "MainGameInstance.h"
#include "RacerController.h"
#include "RacerState.h"
#include "RaceState.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetStringLibrary.h"

ARacingGameMode::ARacingGameMode()
{
	PrimaryActorTick.bCanEverTick = true;
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
		ExpectedPlayersCount = GameInstance->ConnectedPlayersCount;
	}
}

void ARacingGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
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

void ARacingGameMode::PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId,
	FString& ErrorMessage)
{
	Super::PreLogin(Options, Address, UniqueId, ErrorMessage);

	ErrorMessage = "Cannot join a race in progress!";
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

void ARacingGameMode::OnPlayerFinishedRace(FFinishInfo FinishInfo)
{
	
}

void ARacingGameMode::RestartRace()
{
	if(UMainGameInstance* MainGameInstance = Cast<UMainGameInstance>(GetGameInstance()))
	{
		MainGameInstance->TravelToMap(GetWorld()->GetMapName());
	}
}
