// Copyright 2025 Teyon. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/SplineComponent.h"
#include "GameFramework/Actor.h"
#include "SplineMeshPlacer.generated.h"

UCLASS()
class PRAKTYKI_API ASplineMeshPlacer : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASplineMeshPlacer();
	virtual void OnConstruction(const FTransform& Transform) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	USplineComponent* Spline;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UInstancedStaticMeshComponent* InstancedMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMesh* MeshToPlace;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTransform PlacedMeshTransform;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Interval;

	float DistanceBetweenMeshes;
	int32 NumOfMeshes;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
