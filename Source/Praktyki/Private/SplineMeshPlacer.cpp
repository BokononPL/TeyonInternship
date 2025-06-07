// Copyright 2025 Teyon. All Rights Reserved.


#include "SplineMeshPlacer.h"

#include "Components/InstancedStaticMeshComponent.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
ASplineMeshPlacer::ASplineMeshPlacer()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
	Spline = CreateDefaultSubobject<USplineComponent>("Spline");
	InstancedMesh = CreateDefaultSubobject<UInstancedStaticMeshComponent>("InstancedMesh");
	InstancedMesh->SetupAttachment(Spline);

}

void ASplineMeshPlacer::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	
	if(!MeshToPlace) return;
	InstancedMesh->SetStaticMesh(MeshToPlace);
	FVector Bounds = MeshToPlace->GetBoundingBox().Max - MeshToPlace->GetBoundingBox().Min;
	DistanceBetweenMeshes = Bounds.X + Interval;

	InstancedMesh->ClearInstances();
	InstancedMesh->SetRelativeTransform(FTransform::Identity);
	NumOfMeshes = FMath::Floor(Spline->GetSplineLength() / DistanceBetweenMeshes);
	for(int i = 0; i < NumOfMeshes; i++)
	{
		FTransform InstanceTransform = FTransform::Identity;
		float Distance = i * DistanceBetweenMeshes;
		InstanceTransform.SetLocation(Spline->GetLocationAtDistanceAlongSpline(Distance, ESplineCoordinateSpace::Local));
		InstanceTransform.SetRotation(FQuat(UKismetMathLibrary::MakeRotFromX(Spline->GetTangentAtDistanceAlongSpline(Distance, ESplineCoordinateSpace::Local))));
		InstancedMesh->AddInstance(InstanceTransform);
	}
}

// Called when the game starts or when spawned
void ASplineMeshPlacer::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ASplineMeshPlacer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

