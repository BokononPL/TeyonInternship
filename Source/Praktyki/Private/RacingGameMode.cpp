// Fill out your copyright notice in the Description page of Project Settings.


#include "RacingGameMode.h"

#include <string>

#include "MainGameInstance.h"
#include "RaceBotController.h"
#include "RacerController.h"
#include "RacerState.h"
#include "RaceState.h"
#include "Algo/RandomShuffle.h"
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
	UMainGameInstance* GameInstance = Cast<UMainGameInstance>(GetWorld()->GetGameInstance());
    
	if(PlayerControllers.Num() == GameInstance->ConnectedPlayersCount) //all expected players have connected
	{
		Algo::RandomShuffle(PlayerControllers);
		for(int i = 0; i < PlayerControllers.Num(); i++)
		{
			RestartPlayerAtPlayerStart(PlayerControllers[i], ChoosePlayerStart(PlayerControllers[i]));
		}
        
		if(GameInstance->ShouldFillWithBots)
		{
			Algo::RandomShuffle(BotNames);
			int32 BotsAmount = GameInstance->MaxPlayersCount - GameInstance->ConnectedPlayersCount;
			for(int32 i = 0; i < BotsAmount; i++)
			{
				ARacerPawn* NewBot = GetWorld()->SpawnActorDeferred<ARacerPawn>(DefaultPawnClass, FTransform::Identity, this);
				NewBot->SpawnDefaultController();
				if(ARaceBotController* BotController = Cast<ARaceBotController>(NewBot->GetController()))
				{
					BotController->BotName = BotNames[i];
					Bots.Add(BotController);
				}
				NewBot->FinishSpawning(ChoosePlayerStart(NewBot->GetController())->GetTransform());
				if(UAutoDrivingComponent* AutoDrivingComponent = NewBot->FindComponentByClass<UAutoDrivingComponent>())
				{
					AutoDrivingComponent->IsAutoDrivingEnabled = true;
					AutoDrivingComponent->IsDebugEnabled = false;
				}
			}
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
	if(!Player) return nullptr;
    
	if(!Player->StartSpot.IsValid())
	{
		if(!HasInitializedStartsArray)
		{
			UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), StartsArray);
			HasInitializedStartsArray = true;
		}
		if(UsedStartsCount < StartsArray.Num())
		{
			Player->StartSpot = StartsArray[UsedStartsCount];
			UsedStartsCount++;
		}
	}
    
	return Player->StartSpot.Get();
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

void ARacingGameMode::RestartRace()
{
	if(UMainGameInstance* MainGameInstance = Cast<UMainGameInstance>(GetGameInstance()))
	{
		MainGameInstance->TravelToMap(GetWorld()->GetMapName());
	}
}
