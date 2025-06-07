// Copyright 2025 Teyon. All Rights Reserved.


#include "RaceCheckpoint.h"

#include "RacerController.h"
#include "RacerState.h"
#include "RaceState.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetStringLibrary.h"

// Sets default values
ARaceCheckpoint::ARaceCheckpoint()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;
	CheckpointCollider = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CheckpointCollider"));
	CheckpointCollider->SetupAttachment(Mesh);
}

// Called when the game starts or when spawned
void ARaceCheckpoint::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ARaceCheckpoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

bool ARaceCheckpoint::operator<(const ARaceCheckpoint& other) const
{
	return CheckpointIndex < other.CheckpointIndex;
}

