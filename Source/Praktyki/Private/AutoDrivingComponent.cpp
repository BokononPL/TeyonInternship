// Copyright 2025 Teyon. All Rights Reserved.


#include "AutoDrivingComponent.h"

#include "CPPUtils.h"
#include "RacerPawn.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetStringLibrary.h"
#include "Net/UnrealNetwork.h"
#include "Tests/AutomationTestSettings.h"

UAutoDrivingComponent::UAutoDrivingComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(true);
}

void UAutoDrivingComponent::BeginPlay()
{
	Super::BeginPlay();

	TArray<AActor*> PathSplines;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APathSpline::StaticClass(), PathSplines);
	PathSpline = PathSplines.Num() > 0 ? Cast<APathSpline>(PathSplines[0]) : nullptr;
	OwnerActor = GetOwner();
	OwnerMovementComponent = OwnerActor->FindComponentByClass<UChaosVehicleMovementComponent>();
}

void UAutoDrivingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if(!IsAutoDrivingEnabled || !PathSpline || !OwnerActor || !OwnerMovementComponent)
	{
		return;
	}

	if(ARacerPawn* RacerPawn = Cast<ARacerPawn>(OwnerActor))
	{
		if(RacerPawn->IsRespawnAllowed || RacerPawn->IsBotRespawnAllowed)
		{
			RacerPawn->ServerRespawnVehicle();
		}
	}

	float Throttle, Steering;
	CalculateThrottleAndSteering(Throttle, Steering);
	OwnerMovementComponent->SetThrottleInput(FMath::Clamp(Throttle, 0.0f, 1.0f));
	OwnerMovementComponent->SetSteeringInput(Steering);
	
	if(IsDebugEnabled)
	{
		FVector TargetLocationNear = GetTargetLocation(AimDistanceNear);
		FVector TargetLocationFar = GetTargetLocation(AimDistanceFar);
		FVector AimLocationNear = GetAimLocation(AimDistanceNear);
		FVector AimLocationFar = GetAimLocation(AimDistanceFar);
		
		// Target Location (near)
		UKismetSystemLibrary::DrawDebugSphere(GetWorld(), TargetLocationNear, 40, 12, FColor::Yellow);

		// Target Location (far)
		UKismetSystemLibrary::DrawDebugSphere(GetWorld(), TargetLocationFar, 40, 12, FColor::Red);

		// Forward Aim Location (near)
		UKismetSystemLibrary::DrawDebugSphere(GetWorld(), AimLocationNear, 40, 12, FColor::Blue);

		// Forward Aim Location (far)
		UKismetSystemLibrary::DrawDebugSphere(GetWorld(), AimLocationFar, 40, 12, FColor::Purple);
	}
}

void UAutoDrivingComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UAutoDrivingComponent, IsAutoDrivingEnabled);
}

FVector UAutoDrivingComponent::GetAimLocation(float Distance)
{
	if(!OwnerActor) return FVector::ZeroVector;

	return OwnerActor->GetActorLocation() + OwnerActor->GetActorForwardVector() * Distance;
}

FVector UAutoDrivingComponent::GetTargetLocation(float Distance)
{
	if(!PathSpline) return FVector::ZeroVector;
	
	float ClosestSplinePointDistance = PathSpline->SplineComponent->GetDistanceAlongSplineAtLocation(OwnerActor->GetActorLocation(), ESplineCoordinateSpace::World);
	FVector TargetLocation = PathSpline->SplineComponent->GetLocationAtDistanceAlongSpline(FMath::Modulo(ClosestSplinePointDistance + Distance, PathSpline->SplineComponent->GetSplineLength()), ESplineCoordinateSpace::World);

	 return TargetLocation;
}

float UAutoDrivingComponent::ConvertToKMH(float CMS)
{
	return (CMS / 100.0f) * 3.6;
}

float UAutoDrivingComponent::ConvertToCMS(float KMH)
{
	return (KMH * 100.0f) / 3.6;
}

void UAutoDrivingComponent::CalculateThrottleAndSteering(float& Throttle, float& Steering)
{
	FVector TargetLocationNear = GetTargetLocation(AimDistanceNear);
	FVector TargetLocationFar = GetTargetLocation(AimDistanceFar);
	FVector AimLocationNear = GetAimLocation(AimDistanceNear);
	FVector AimLocationFar = GetAimLocation(AimDistanceFar);
	
	float DistanceDifferenceMultiplier = FMath::Pow(FMath::Clamp((AimLocationNear - TargetLocationNear).Length() / DampeningDistance, SteeringMultiplier, 1.0f), SteeringExponent);
	float AngleDifferenceNear = UKismetMathLibrary::NormalizedDeltaRotator(
		UKismetMathLibrary::FindLookAtRotation(OwnerActor->GetActorLocation(), AimLocationNear),
		UKismetMathLibrary::FindLookAtRotation(OwnerActor->GetActorLocation(), TargetLocationNear)).Yaw;
	Steering = UKismetMathLibrary::MapRangeClamped(AngleDifferenceNear, -MaxSteeringPowerAngle, MaxSteeringPowerAngle, 1.0f, -1.0f) * DistanceDifferenceMultiplier;

	float LocalMaxSpeed;
	if(FMath::IsNearlyEqual((TargetLocationNear - AimLocationNear).Length(), (TargetLocationFar - AimLocationFar).Length(), 150.0f))
	{
		LocalMaxSpeed = MaxSpeed;
	}
	else
	{
		LocalMaxSpeed = MinSpeed;
	}
	float CurrentSpeed = OwnerMovementComponent->GetForwardSpeed();
	Throttle = ConvertToKMH(CurrentSpeed) > LocalMaxSpeed ? 0.0f : 1.0f;
}