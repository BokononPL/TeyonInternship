// Copyright 2025 Teyon. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/SplineComponent.h"
#include "GameFramework/Actor.h"
#include "PathSpline.generated.h"

UCLASS()
class PRAKTYKI_API APathSpline : public AActor
{
	GENERATED_BODY()
	
public:	
	APathSpline();

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	USplineComponent* SplineComponent;
};
