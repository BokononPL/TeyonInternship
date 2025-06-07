// Copyright 2025 Teyon. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AutoDrivingComponent.h"
#include "WheeledVehiclePawn.h"
#include "CPPUtils.h"
#include "PlayerScore.h"
#include "EnhancedInputComponent.h"
#include "ChaosWheeledVehicleMovementComponent.h"
#include "RaceCheckpoint.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "RacerPawn.generated.h"

/**
 * 
 */
UCLASS()
class PRAKTYKI_API ARacerPawn : public AWheeledVehiclePawn
{
	GENERATED_BODY()

public:
	ARacerPawn();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	void SetupCarParts();

	TObjectPtr<UChaosWheeledVehicleMovementComponent> VehicleMovementComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UAutoDrivingComponent* AutoDrivingComponent;
	
	class ARacerState* RacerState = nullptr;

#pragma region Inputs
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Configuration|Input")
	UInputAction* Throttle_Action;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Configuration|Input")
	UInputAction* Brake_Action;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Configuration|Input")
	UInputAction* HandBrake_Action;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Configuration|Input")
	UInputAction* Steer_Action;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Configuration|Input")
	UInputAction* LookAround_Action;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Configuration|Input")
	UInputAction* ToggleRightTurnIndicator_Action;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Configuration|Input")
	UInputAction* ToggleLeftTurnIndicator_Action;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Configuration|Input")
	UInputAction* ToggleCamera_Action;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Configuration|Input")
	UInputAction* OpenMenu_Action;
	
protected:
	void Throttle(const FInputActionValue& Value);
	void Brake(const FInputActionValue& Value);
	void HandBrakeStarted(const FInputActionValue& Value);
	void HandBrakeCompleted(const FInputActionValue& Value);
	void Steer(const FInputActionValue& Value);
	void LookAround(const FInputActionValue& Value);
	void ToggleRightTurnIndicator(const FInputActionValue& Value);
	void ToggleLeftTurnIndicator(const FInputActionValue& Value);
	void ToggleCamera(const FInputActionValue& Value);
	void OpenMenu(const FInputActionValue& Value);

#pragma endregion
	
public:
	UPROPERTY(BlueprintReadOnly, Replicated)
	float TurnIndicatorSetting = 0.0f;

	UFUNCTION(Server, Reliable)
	void ServerSetTurnIndicator(float Value);

private:
	TArray<UCameraComponent*> Cameras;
	int CurrentCameraIndex;
	void SelectCamera(int Index);
	
public:
#pragma region CarParts
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CarParts")
	UStaticMeshComponent* RightDoor;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CarParts")
	UStaticMeshComponent* LeftDoor;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CarParts")
	UStaticMeshComponent* RearBoot;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CarParts")
	UStaticMeshComponent* RearBumper;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CarParts")
	UStaticMeshComponent* FrontBumper;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CarParts")
	UStaticMeshComponent* RearDiffuser;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CarParts")
	UStaticMeshComponent* RightFender;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CarParts")
	UStaticMeshComponent* LeftFender;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CarParts")
	UStaticMeshComponent* FrontHood;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CarParts")
	UStaticMeshComponent* RearSpoiler;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CarParts")
	UStaticMeshComponent* MainBody;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CarParts")
	UStaticMeshComponent* Interior;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CarParts")
	UStaticMeshComponent* EngineComponents;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CarParts")
	UStaticMeshComponent* CockpitConsole;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CarParts")
	UStaticMeshComponent* Net;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CarParts")
	UStaticMeshComponent* SeatNetClamps;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CarParts")
	UStaticMeshComponent* SteeringWheel;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CarParts")
	UStaticMeshComponent* Wiper;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CarParts")
	UStaticMeshComponent* AccelerationPedal;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CarParts")
	UStaticMeshComponent* BrakePedal;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CarParts")
	UStaticMeshComponent* RightWingMirror;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CarParts")
	UStaticMeshComponent* LeftWingMirror;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CarParts")
	UStaticMeshComponent* FrontRightWheel;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CarParts")
	UStaticMeshComponent* FrontRightBrakeDisc;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CarParts")
	UStaticMeshComponent* FrontRightCaliper;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CarParts")
	UStaticMeshComponent* FrontLeftWheel;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CarParts")
	UStaticMeshComponent* FrontLeftBrakeDisc;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CarParts")
	UStaticMeshComponent* FrontLeftCaliper;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CarParts")
	UStaticMeshComponent* RearRightWheel;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CarParts")
	UStaticMeshComponent* RearRightBrakeDisc;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CarParts")
	UStaticMeshComponent* RearRightCaliper;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CarParts")
	UStaticMeshComponent* RearLeftWheel;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CarParts")
	UStaticMeshComponent* RearLeftBrakeDisc;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CarParts")
	UStaticMeshComponent* RearLeftCaliper;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CarParts")
	UStaticMeshComponent* Windshield;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CarParts")
	UMaterialInstanceDynamic* RearBumperLights_MI;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CarParts")
	TArray<UStaticMeshComponent*> PaintedCarParts;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CarParts")
	TArray<UMaterialInstanceDynamic*> PaintedCarParts_MIs;

#pragma endregion

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cameras")
	USpringArmComponent* SpectatorSpringArm;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cameras")
	UCameraComponent* SpectatorCamera;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cameras")
	UCameraComponent* HoodCamera;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cameras")
	UCameraComponent* InteriorCamera;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cameras")
	float MaxCameraPitch;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cameras")
	float MinCameraPitch;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sound")
	UAudioComponent* EngineSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sound")
	float EngineSoundPitchMin;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sound")
	float EngineSoundPitchMax;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<ARaceCheckpoint*> RaceCheckpoints;

	UFUNCTION()
	void CheckpointCollision(AActor* ThisActor, AActor* OtherActor);

	UFUNCTION(BlueprintCallable)
	void SetDrivingEnabled(bool IsEnabled);

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Replicated)
	bool IsDrivingEnabled;

	float RespawnAllowedAlpha = 0.0f;
	bool IsRespawnAllowed = false;

	UFUNCTION(BlueprintCallable)
	bool CheckIsOffTrack();

	UFUNCTION(BlueprintCallable)
	bool CheckAreWheelsOffGround();
};
