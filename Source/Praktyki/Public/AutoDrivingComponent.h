// Copyright 2025 Teyon. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ChaosVehicleMovementComponent.h"
#include "PathSpline.h"
#include "Components/ActorComponent.h"
#include "AutoDrivingComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PRAKTYKI_API UAutoDrivingComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UAutoDrivingComponent();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(BlueprintReadWrite)
	APathSpline* PathSpline;

	UPROPERTY(BlueprintReadWrite)
	AActor* OwnerActor;

	UPROPERTY(BlueprintReadWrite)
	UChaosVehicleMovementComponent* OwnerMovementComponent;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	float AimDistanceNear = 1500;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	float AimDistanceFar = 3000;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	float MinSteeringPowerAngle = 0.0f;
	
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	float MaxSteeringPowerAngle = 60.0f;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	float DampeningDistance = 400.0f;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	float SteeringMultiplier = 0.1f;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	float SteeringExponent = 2.0f;
	
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	bool IsDebugEnabled = false;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	float MinSpeed = 30.0f;
	
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	float MaxSpeed = 200.0f;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	float MinSpeedAngle = 45.0f;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	float MaxSpeedAngle = 0.0f;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	float SpeedAdjustmentRange = 30.0f;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Replicated)
	bool IsAutoDrivingEnabled = false;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FVector GetAimLocation(float Distance);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FVector GetTargetLocation(float Distance);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	float ConvertToKMH(float CMS);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	float ConvertToCMS(float KMH);

	UFUNCTION(BlueprintCallable)
	void CalculateThrottleAndSteering(float& Throttle, float& Steering);
};
