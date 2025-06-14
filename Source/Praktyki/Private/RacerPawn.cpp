// Copyright 2025 Teyon. All Rights Reserved.


#include "RacerPawn.h"

#include "GeometryTypes.h"
#include "RaceBotController.h"
#include "RaceCheckpoint.h"
#include "RacerController.h"
#include "RacerState.h"
#include "RaceState.h"
#include "RacingGameMode.h"
#include "Components/AudioComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "GameFramework/InputSettings.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetStringLibrary.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Net/UnrealNetwork.h"

ARacerPawn::ARacerPawn()
{
    SetupCarParts();
    Cameras.Add(SpectatorCamera);
    Cameras.Add(HoodCamera);
    Cameras.Add(InteriorCamera);
    
    VehicleMovementComponent = CastChecked<UChaosWheeledVehicleMovementComponent>(GetVehicleMovement());
    AutoDrivingComponent = CreateDefaultSubobject<UAutoDrivingComponent>("AutoDrivingComponent");

    EngineSound = CreateDefaultSubobject<UAudioComponent>("Engine Sound");
    EngineSound->SetupAttachment(GetMesh(), "EngineExtAudioSourceComponentSocket");
    
    GetMesh()->SetSimulatePhysics(true);
    GetMesh()->SetCollisionProfileName(FName("Vehicle"));
}

void ARacerPawn::BeginPlay()
{
    Super::BeginPlay();
    SelectCamera(0);
    RearBumperLights_MI = RearBumper->CreateDynamicMaterialInstance(3);
    OnActorBeginOverlap.AddDynamic(this, &ARacerPawn::CheckpointCollision);

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

    SetDrivingEnabled(false);
}

void ARacerPawn::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    
    if(RearBumperLights_MI)
    {
        RearBumperLights_MI->SetScalarParameterValue("Brake",
            //going forward but trying to stop/go backward
            (VehicleMovementComponent->GetBrakeInput() > 0.01f && VehicleMovementComponent->GetForwardSpeed() > -5.0f) ||
            //going backward but trying to stop/go forward
            (VehicleMovementComponent->GetThrottleInput() > 0.01f && VehicleMovementComponent->GetForwardSpeed() < -5.0f) ||
            //using handbrake
            VehicleMovementComponent->GetHandbrakeInput() ||
            //holding throttle and brake at the same time
            (VehicleMovementComponent->GetBrakeInput() > 0.01f && VehicleMovementComponent->GetThrottleInput() > 0.01f)
            ? 1.0f : 0.0f);
        RearBumperLights_MI->SetScalarParameterValue("TurnIndicator", TurnIndicatorSetting);
    }

    if(!RacerState) //check if we're ready in multiplayer
    {
        RacerState = Cast<ARacerState>(GetPlayerState());
        if(RacerState)
        {
            for(UStaticMeshComponent* PaintedCarPart : PaintedCarParts)
            {
                if(!PaintedCarPart) continue;
                PaintedCarParts_MIs.Add(PaintedCarPart->CreateDynamicMaterialInstance(0));
            }
            for(UMaterialInstanceDynamic* MI : PaintedCarParts_MIs)
            {
                if(!MI) continue;
                MI->SetScalarParameterValue("AlbedoAlpha", RacerState->RacerInfo.ShouldUseDefaultPaintjob ? 1 : 0);
                MI->SetVectorParameterValue("PaintColor", RacerState->RacerInfo.CarColor);
            }
        }
    }
    if(RacerState)
    {
        float NewPitchMult = UKismetMathLibrary::MapRangeClamped(VehicleMovementComponent->GetEngineRotationSpeed(), 0.0f, 5000.0f, EngineSoundPitchMin, EngineSoundPitchMax);
        EngineSound->SetPitchMultiplier(NewPitchMult);
    }
    if(HasAuthority() && RacerState && CheckIsOffTrack())
    {
        if(ARaceState* RaceState = Cast<ARaceState>(GetWorld()->GetGameState()))
        {
            if(RaceState->ShouldInvalidateLaps)
            {
                RacerState->IsLapInvalidated = true;
            }
        }
    }
    if(HasAuthority())
    {
        if(CheckAreWheelsOffGround())
        {
            RespawnAllowedAlpha += GetWorld()->GetDeltaSeconds() * 0.2f;
        }
        else
        {
            RespawnAllowedAlpha -= GetWorld()->GetDeltaSeconds() * 1.0f;
        }
        RespawnAllowedAlpha = FMath::Clamp(RespawnAllowedAlpha, 0.0f, 1.0f);
        if(RespawnAllowedAlpha >= 0.999f)
        {
            IsRespawnAllowed = true;
        }
        else if(RespawnAllowedAlpha <= 0.001f)
        {
            IsRespawnAllowed = false;
        }
        if(ARaceBotController* RBC = Cast<ARaceBotController>(GetController()))
        {
            if(FMath::Abs(VehicleMovementComponent->GetForwardSpeed()) < 50.0f)
            {
                if(ARaceState* RaceState = Cast<ARaceState>(GetWorld()->GetGameState()))
                {
                    if(RaceState->GetServerWorldTimeSeconds() > RaceState->RaceStartTime)
                    {
                        BotStuckAlpha += GetWorld()->GetDeltaSeconds() * 0.15f;
                    }
                }
            }
            else
            {
                BotStuckAlpha -= GetWorld()->GetDeltaSeconds() * 2.0f;
            }
            BotStuckAlpha = FMath::Clamp(BotStuckAlpha, 0.0f, 1.0f);
            if(BotStuckAlpha >= 0.999f)
            {
                IsBotRespawnAllowed = true;
            }
            else if(BotStuckAlpha <= 0.001f)
            {
                IsBotRespawnAllowed = false;
            }
        }
    }
}

void ARacerPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    if(IsLocallyControlled())
    {
        if(UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(PlayerInputComponent))
        {
            if(Drive_Action)
            {
                EIC->BindAction(Drive_Action, ETriggerEvent::Triggered, this, &ARacerPawn::Drive);
                EIC->BindAction(Drive_Action, ETriggerEvent::Completed, this, &ARacerPawn::Drive);
            }
            if(Throttle_Action)
            {
                EIC->BindAction(Throttle_Action, ETriggerEvent::Triggered, this, &ARacerPawn::Throttle);
                EIC->BindAction(Throttle_Action, ETriggerEvent::Completed, this, &ARacerPawn::Throttle);
            }
            if(Brake_Action)
            {
                EIC->BindAction(Brake_Action, ETriggerEvent::Triggered, this, &ARacerPawn::Brake);
                EIC->BindAction(Brake_Action, ETriggerEvent::Completed, this, &ARacerPawn::Brake);
            }
            if(ToggleLeftTurnIndicator_Action)
            {
                EIC->BindAction(HandBrake_Action, ETriggerEvent::Started, this, &ARacerPawn::HandBrakeStarted);
            }
            if(ToggleLeftTurnIndicator_Action)
            {
                EIC->BindAction(HandBrake_Action, ETriggerEvent::Completed, this, &ARacerPawn::HandBrakeCompleted);
            }
            if(Steer_Action)
            {
                EIC->BindAction(Steer_Action, ETriggerEvent::Triggered, this, &ARacerPawn::Steer);
                EIC->BindAction(Steer_Action, ETriggerEvent::Completed, this, &ARacerPawn::Steer);
            }
            if(LookAround_Action)
            {
                EIC->BindAction(LookAround_Action, ETriggerEvent::Triggered, this, &ARacerPawn::LookAround);
            }
            if(ToggleRightTurnIndicator_Action)
            {
                EIC->BindAction(ToggleRightTurnIndicator_Action, ETriggerEvent::Started, this, &ARacerPawn::ToggleRightTurnIndicator);
            }
            if(ToggleLeftTurnIndicator_Action)
            {
                EIC->BindAction(ToggleLeftTurnIndicator_Action, ETriggerEvent::Started, this, &ARacerPawn::ToggleLeftTurnIndicator);
            }
            if(ToggleCamera_Action)
            {
                EIC->BindAction(ToggleCamera_Action, ETriggerEvent::Started, this, &ARacerPawn::ToggleCamera);
            }
            if(OpenMenu_Action)
            {
                EIC->BindAction(OpenMenu_Action, ETriggerEvent::Started, this, &ARacerPawn::OpenMenu);
            }
            if(Respawn_Action)
            {
                EIC->BindAction(Respawn_Action, ETriggerEvent::Started, this, &ARacerPawn::RespawnVehicle);
            }
        }
    }
}

void ARacerPawn::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(ARacerPawn, TurnIndicatorSetting);
    DOREPLIFETIME(ARacerPawn, IsDrivingEnabled);
    DOREPLIFETIME(ARacerPawn, IsRespawnAllowed);
    DOREPLIFETIME(ARacerPawn, IsBotRespawnAllowed);
}

void ARacerPawn::SetupCarParts()
{
    RightDoor = CreateDefaultSubobject<UStaticMeshComponent>("Right Door");
    RightDoor->SetupAttachment(GetMesh(), "door_right");
    PaintedCarParts.Add(RightDoor);
    
    LeftDoor = CreateDefaultSubobject<UStaticMeshComponent>("Left Door");
    LeftDoor->SetupAttachment(GetMesh(), "door_left");
    PaintedCarParts.Add(LeftDoor);
    
    RearBoot = CreateDefaultSubobject<UStaticMeshComponent>("Rear Boot");
    RearBoot->SetupAttachment(GetMesh(), "boot_rear");
    PaintedCarParts.Add(RearBoot);
    
    RearBumper = CreateDefaultSubobject<UStaticMeshComponent>("Rear Bumper");
    RearBumper->SetupAttachment(GetMesh(), "bumper_rear");
    PaintedCarParts.Add(RearBumper);
    
    FrontBumper = CreateDefaultSubobject<UStaticMeshComponent>("Front Bumper");
    FrontBumper->SetupAttachment(GetMesh(), "bumper_front");
    PaintedCarParts.Add(FrontBumper);

    RearDiffuser = CreateDefaultSubobject<UStaticMeshComponent>("Rear Diffuser");
    RearDiffuser->SetupAttachment(GetMesh(), "diffuser_back");

    RightFender = CreateDefaultSubobject<UStaticMeshComponent>("Right Fender");
    RightFender->SetupAttachment(GetMesh(), "fender_right");
    PaintedCarParts.Add(RightFender);

    LeftFender = CreateDefaultSubobject<UStaticMeshComponent>("Left Fender");
    LeftFender->SetupAttachment(GetMesh(), "fender_left");
    PaintedCarParts.Add(LeftFender);
    
    FrontHood = CreateDefaultSubobject<UStaticMeshComponent>("Front Hood");
    FrontHood->SetupAttachment(GetMesh(), "hood_front");
    PaintedCarParts.Add(FrontHood);
    
    RearSpoiler = CreateDefaultSubobject<UStaticMeshComponent>("Rear Spoiler");
    RearSpoiler->SetupAttachment(GetMesh(), "spoiler_back");
    PaintedCarParts.Add(RearSpoiler);

    MainBody = CreateDefaultSubobject<UStaticMeshComponent>("Main Body");
    MainBody->SetupAttachment(GetMesh());
    PaintedCarParts.Add(MainBody);

    Interior = CreateDefaultSubobject<UStaticMeshComponent>("Interior");
    Interior->SetupAttachment(GetMesh());

    EngineComponents = CreateDefaultSubobject<UStaticMeshComponent>("Engine Components");
    EngineComponents->SetupAttachment(GetMesh());

    CockpitConsole = CreateDefaultSubobject<UStaticMeshComponent>("CockpitConsole");
    CockpitConsole->SetupAttachment(GetMesh());

    Net = CreateDefaultSubobject<UStaticMeshComponent>("Net");
    Net->SetupAttachment(GetMesh());

    SeatNetClamps = CreateDefaultSubobject<UStaticMeshComponent>("Seat Net Clamps");
    SeatNetClamps->SetupAttachment(GetMesh());

    SteeringWheel = CreateDefaultSubobject<UStaticMeshComponent>("Steering Wheel");
    SteeringWheel->SetupAttachment(Interior, "SteeringWheel");

    Wiper = CreateDefaultSubobject<UStaticMeshComponent>("Wiper");
    Wiper->SetupAttachment(Interior, "Wiper");

    AccelerationPedal = CreateDefaultSubobject<UStaticMeshComponent>("Acceleration Pedal");
    AccelerationPedal->SetupAttachment(Interior, "Pedal_acceleration");

    BrakePedal = CreateDefaultSubobject<UStaticMeshComponent>("Brake Pedal");
    BrakePedal->SetupAttachment(Interior, "Pedal_brake");

    RightWingMirror = CreateDefaultSubobject<UStaticMeshComponent>("Right Wing Mirror");
    RightWingMirror->SetupAttachment(GetMesh(), "wing_mirror_right");
    PaintedCarParts.Add(RightWingMirror);

    LeftWingMirror = CreateDefaultSubobject<UStaticMeshComponent>("Left Wing Mirror");
    LeftWingMirror->SetupAttachment(GetMesh(), "wing_mirror_left");
    PaintedCarParts.Add(LeftWingMirror);

    FrontRightWheel = CreateDefaultSubobject<UStaticMeshComponent>("Front Right Wheel");
    FrontRightWheel->SetupAttachment(GetMesh(), "wheel_front_right_spin");

    FrontRightBrakeDisc = CreateDefaultSubobject<UStaticMeshComponent>("Front Right Brake Disc");
    FrontRightBrakeDisc->SetupAttachment(GetMesh(), "wheel_front_right_spin");

    FrontRightCaliper = CreateDefaultSubobject<UStaticMeshComponent>("Front Right Caliper");
    FrontRightCaliper->SetupAttachment(GetMesh(), "suspension_front_right");

    FrontRightWheelBlur = CreateDefaultSubobject<UStaticMeshComponent>("Front Right Wheel Blur");
    FrontRightWheelBlur->SetupAttachment(GetMesh(), "wheel_front_right_spin");

    FrontLeftWheel = CreateDefaultSubobject<UStaticMeshComponent>("Front Left Wheel");
    FrontLeftWheel->SetupAttachment(GetMesh(), "wheel_front_left_spin");

    FrontLeftBrakeDisc = CreateDefaultSubobject<UStaticMeshComponent>("Front Left Brake Disc");
    FrontLeftBrakeDisc->SetupAttachment(GetMesh(), "wheel_front_left_spin");

    FrontLeftCaliper = CreateDefaultSubobject<UStaticMeshComponent>("Front Left Caliper");
    FrontLeftCaliper->SetupAttachment(GetMesh(), "suspension_front_left");

    FrontLeftWheelBlur = CreateDefaultSubobject<UStaticMeshComponent>("Front Left Wheel Blur");
    FrontLeftWheelBlur->SetupAttachment(GetMesh(), "wheel_front_left_spin");

    RearRightWheel = CreateDefaultSubobject<UStaticMeshComponent>("Rear Right Wheel");
    RearRightWheel->SetupAttachment(GetMesh(), "wheel_back_right_spin");

    RearRightBrakeDisc = CreateDefaultSubobject<UStaticMeshComponent>("Rear Right Brake Disc");
    RearRightBrakeDisc->SetupAttachment(GetMesh(), "wheel_back_right_spin");

    RearRightCaliper = CreateDefaultSubobject<UStaticMeshComponent>("Rear Right Caliper");
    RearRightCaliper->SetupAttachment(GetMesh(), "suspension_back_right");

    RearRightWheelBlur = CreateDefaultSubobject<UStaticMeshComponent>("Rear Right Wheel Blur");
    RearRightWheelBlur->SetupAttachment(GetMesh(), "wheel_back_right_spin");

    RearLeftWheel = CreateDefaultSubobject<UStaticMeshComponent>("Rear Left Wheel");
    RearLeftWheel->SetupAttachment(GetMesh(), "wheel_back_left_spin");

    RearLeftBrakeDisc = CreateDefaultSubobject<UStaticMeshComponent>("Rear Left Brake Disc");
    RearLeftBrakeDisc->SetupAttachment(GetMesh(), "wheel_back_left_spin");

    RearLeftCaliper = CreateDefaultSubobject<UStaticMeshComponent>("Rear Left Caliper");
    RearLeftCaliper->SetupAttachment(GetMesh(), "suspension_back_left");

    RearLeftWheelBlur = CreateDefaultSubobject<UStaticMeshComponent>("Rear Left Wheel Blur");
    RearLeftWheelBlur->SetupAttachment(GetMesh(), "wheel_back_left_spin");

    Windshield = CreateDefaultSubobject<UStaticMeshComponent>("Windshield");
    Windshield->SetupAttachment(GetMesh());

    SpectatorSpringArm = CreateDefaultSubobject<USpringArmComponent>("Spectator Spring Arm");
    SpectatorSpringArm->SetupAttachment(GetMesh());
    
    SpectatorCamera = CreateDefaultSubobject<UCameraComponent>("Spectator Camera");
    SpectatorCamera->SetupAttachment(SpectatorSpringArm);

    HoodCamera = CreateDefaultSubobject<UCameraComponent>("Hood Camera");
    HoodCamera->SetupAttachment(GetMesh(), "HoodGameplayCamera");

    InteriorCamera = CreateDefaultSubobject<UCameraComponent>("Interior Camera");
    InteriorCamera->SetupAttachment(GetMesh(), "CarInteriorGameplayCamera");
}

void ARacerPawn::SelectCamera(int Index)
{
    CurrentCameraIndex = Index;
    for(int i = 0; i < Cameras.Num(); i++)
    {
        if(i != CurrentCameraIndex)
        {
            Cameras[i]->Deactivate();
        }
        else
        {
            Cameras[i]->Activate();
        }
    }
}

void ARacerPawn::Drive(const FInputActionValue& Value)
{
    VehicleMovementComponent->SetThrottleInput(FMath::Clamp(Value.Get<float>(), 0.0f, 1.0f));
    VehicleMovementComponent->SetBrakeInput(FMath::Clamp(-Value.Get<float>(), 0.0f, 1.0f));
}

void ARacerPawn::Throttle(const FInputActionValue& Value)
{
    VehicleMovementComponent->SetThrottleInput(Value.Get<float>());
}

void ARacerPawn::Brake(const FInputActionValue& Value)
{
    VehicleMovementComponent->SetBrakeInput(Value.Get<float>());
}

void ARacerPawn::HandBrakeStarted(const FInputActionValue& Value)
{
    VehicleMovementComponent->SetHandbrakeInput(true);
}

void ARacerPawn::HandBrakeCompleted(const FInputActionValue& Value)
{
    VehicleMovementComponent->SetHandbrakeInput(false);
}

void ARacerPawn::Steer(const FInputActionValue& Value)
{
    float SteerValue = Value.Get<float>();
    VehicleMovementComponent->SetSteeringInput(SteerValue);
}

void ARacerPawn::LookAround(const FInputActionValue& Value)
{
    FVector2d LookAroundValue = Value.Get<FVector2d>();
    if(SpectatorSpringArm)
    {
        FRotator NewRotation = SpectatorSpringArm->GetRelativeRotation();
        NewRotation.Pitch = FMath::Clamp(NewRotation.Pitch + LookAroundValue.Y, MinCameraPitch, MaxCameraPitch);
        NewRotation.Yaw += LookAroundValue.X;
        NewRotation.Roll = 0.0f;
        SpectatorSpringArm->SetRelativeRotation(NewRotation);
    }
}

void ARacerPawn::ToggleRightTurnIndicator(const FInputActionValue& Value)
{
    if(FMath::IsNearlyEqual(TurnIndicatorSetting, 1.0f))
    {
        TurnIndicatorSetting = 0.0f;
    }
    else
    {
        TurnIndicatorSetting = 1.0f;
    }
    if(!HasAuthority())
    {
        ServerSetTurnIndicator(TurnIndicatorSetting);
    }
}

void ARacerPawn::ToggleLeftTurnIndicator(const FInputActionValue& Value)
{
    if(FMath::IsNearlyEqual(TurnIndicatorSetting, -1.0f))
    {
        TurnIndicatorSetting = 0.0f;
    }
    else
    {
        TurnIndicatorSetting = -1.0f;
    }
    if(!HasAuthority())
    {
        ServerSetTurnIndicator(TurnIndicatorSetting);
    }
}

void ARacerPawn::ToggleCamera(const FInputActionValue& Value)
{
    if(CurrentCameraIndex == Cameras.Num() - 1)
    {
        CurrentCameraIndex = 0;
    }
    else
    {
        CurrentCameraIndex++;
    }

    SelectCamera(CurrentCameraIndex);
}

void ARacerPawn::OpenMenu(const FInputActionValue& Value)
{
    if(ARacerController* RacerController = Cast<ARacerController>(GetController()))
    {
        RacerController->OpenMenu();
    }
}

void ARacerPawn::RespawnVehicle(const FInputActionValue& Value)
{
    if(!IsRespawnAllowed) return;
    ServerRespawnVehicle();
}

void ARacerPawn::ServerRespawnVehicle_Implementation()
{
    if(AController* C = GetController())
    {
        if(IsRespawnAllowed)
        {
            GetMesh()->SetSimulatePhysics(false);
            SetActorLocationAndRotation(C->StartSpot->GetActorLocation() + FVector(0.0f, 0.0f, 600.0f), C->StartSpot->GetActorRotation(), false, nullptr, ETeleportType::ResetPhysics);
            GetMesh()->SetSimulatePhysics(true);
            if(RacerState)
            {
                RacerState->CurrentCheckpoint = 0;
                RacerState->IsLapInvalidated = false;
                IsRespawnAllowed = false;
                RespawnAllowedAlpha = 0.0f;
                IsBotRespawnAllowed = false;
                BotStuckAlpha = 0.0f;
            }
        }
        //respawning for bots
        if(ARaceBotController* RBC = Cast<ARaceBotController>(C))
        {
            if(IsRespawnAllowed || IsBotRespawnAllowed)
            {
                GetMesh()->SetSimulatePhysics(false);
                SetActorLocationAndRotation(C->StartSpot->GetActorLocation() + FVector(0.0f, 0.0f, 600.0f), C->StartSpot->GetActorRotation(), false, nullptr, ETeleportType::ResetPhysics);
                GetMesh()->SetSimulatePhysics(true);
                RBC->CurrentCheckpoint = 0;
                RBC->IsLapInvalidated = false;
                IsRespawnAllowed = false;
                RespawnAllowedAlpha = 0.0f;
                IsBotRespawnAllowed = false;
                BotStuckAlpha = 0.0f;
            }
        }
    }
}


void ARacerPawn::ServerSetTurnIndicator_Implementation(float Value)
{
    TurnIndicatorSetting = Value;
}

void ARacerPawn::CheckpointCollision(AActor* ThisActor, AActor* OtherActor)
{
    if(HasAuthority())
    {
        if(ARaceCheckpoint* Checkpoint = Cast<ARaceCheckpoint>(OtherActor))
        {
            if(ARaceState* RaceState = Cast<ARaceState>(UGameplayStatics::GetGameState(GetWorld())))
            {
                if(RaceState->IsRaceOver) return;
                
                if(RacerState)
                {
                    if(Checkpoint->CheckpointIndex != 0)
                    {
                        if(Checkpoint->CheckpointIndex == RacerState->CurrentCheckpoint + 1)
                        {
                            RacerState->CurrentCheckpoint = Checkpoint->CheckpointIndex;
    
                        }
                    }
                    else
                    {
                        if(RacerState->CurrentCheckpoint == RaceState->MaxCheckpointIndex)
                        {
                            RacerState->CurrentCheckpoint = Checkpoint->CheckpointIndex;
                            if(RacerState->IsLapInvalidated)
                            {
                                RacerState->IsLapInvalidated = false;
                            }
                            else
                            {
                                RacerState->CurrentLap++;
                                TArray<float> tmp = RacerState->LapTimes;
                                tmp.Add(RaceState->GetServerWorldTimeSeconds() - RaceState->RaceStartTime);
                                RacerState->LapTimes = tmp;

                                if(ARacerController* RacerController = Cast<ARacerController>(GetController()))
                                {
                                    RacerController->AddLapTime(RacerState->CurrentLap, RacerState->LapTimes.Last());
                                }
                                
                                if(RacerState->BestLapTime < 0.0f)
                                {
                                    RacerState->BestLapTime = RacerState->LapTimes[0];
                                }
                                else
                                {
                                    float CurrentLapTime = RaceState->GetServerWorldTimeSeconds() - RacerState->CurrentLapStartTime;
                                    if(CurrentLapTime < RacerState->BestLapTime)
                                    {
                                        RacerState->BestLapTime = CurrentLapTime;
                                    }
                                }
                            
                                if(RacerState->CurrentLap == RaceState->MaxLaps && (RaceState->GameType != GameTypeEnum::Practice))
                                {
                                    RaceState->FinishTimes.Add(FFinishInfo(RacerState->RacerInfo.PlayerName, RacerState->LapTimes.Last()));
                                    AutoDrivingComponent->IsAutoDrivingEnabled = true;
                                    if(ARacerController* RacerController = Cast<ARacerController>(GetController()))
                                    {
                                        RacerController->Client_OnRaceFinished(RaceState->FinishTimes.Num(), RacerState->LapTimes);
                                    }
                                }
                            }
                            RacerState->CurrentLapStartTime = RaceState->GetServerWorldTimeSeconds();
                        }
                    }
                }
                else if(ARaceBotController* Bot = Cast<ARaceBotController>(GetController()))
                {
                    if(Checkpoint->CheckpointIndex != 0)
                    {
                        if(Checkpoint->CheckpointIndex == Bot->CurrentCheckpoint + 1)
                        {
                            Bot->CurrentCheckpoint = Checkpoint->CheckpointIndex;
    
                        }
                    }
                    else
                    {
                        if(Bot->CurrentCheckpoint == RaceState->MaxCheckpointIndex)
                        {
                            Bot->CurrentCheckpoint = Checkpoint->CheckpointIndex;
                            if(Bot->IsLapInvalidated)
                            {
                                Bot->IsLapInvalidated = false;
                            }
                            else
                            {
                                Bot->CurrentLap++;
                                TArray<float> tmp = Bot->LapTimes;
                                tmp.Add(RaceState->GetServerWorldTimeSeconds() - RaceState->RaceStartTime);
                                Bot->LapTimes = tmp;

                                if(Bot->CurrentLap == RaceState->MaxLaps)
                                {
                                    RaceState->FinishTimes.Add(FFinishInfo(Bot->BotName, Bot->LapTimes.Last()));
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}


void ARacerPawn::SetDrivingEnabled(bool IsEnabled)
{
    IsDrivingEnabled = IsEnabled;
    if(VehicleMovementComponent)
    {
        if(IsEnabled)
        {
            VehicleMovementComponent->SetUseAutomaticGears(true);
            VehicleMovementComponent->bReverseAsBrake = true;
            VehicleMovementComponent->bThrottleAsBrake = true;
        }
        else
        {
            VehicleMovementComponent->SetUseAutomaticGears(false);
            VehicleMovementComponent->SetTargetGear(0, true);
            VehicleMovementComponent->bReverseAsBrake = false;
            VehicleMovementComponent->bThrottleAsBrake = false;
        }
    }
}

bool ARacerPawn::CheckIsOffTrack()
{
    if(!VehicleMovementComponent) return false;
    ARaceState* RaceState = Cast<ARaceState>(GetWorld()->GetGameState());
    if(!RaceState) return false;
    for(int i = 0; i < VehicleMovementComponent->GetNumWheels() - 1; i++)
    {
        if(!RaceState->OffTrackMaterials.Contains(VehicleMovementComponent->GetWheelState(i).PhysMaterial))
        {
            return false;
        }
    }
    return true;
}

bool ARacerPawn::CheckAreWheelsOffGround()
{
    if(!VehicleMovementComponent) return false;
    ARaceState* RaceState = Cast<ARaceState>(GetWorld()->GetGameState());
    if(!RaceState) return false;
    int WheelsOffGround = 0;
    for(int i = 0; i < VehicleMovementComponent->GetNumWheels() - 1; i++)
    {
        if(!(RaceState->OffTrackMaterials.Contains(VehicleMovementComponent->GetWheelState(i).PhysMaterial)) &&
            !(RaceState->OnTrackMaterials.Contains(VehicleMovementComponent->GetWheelState(i).PhysMaterial)))
        {
            WheelsOffGround++;
        }
        if(WheelsOffGround >= 2)
        {
            return true;
        }
    }
    return false;
}
