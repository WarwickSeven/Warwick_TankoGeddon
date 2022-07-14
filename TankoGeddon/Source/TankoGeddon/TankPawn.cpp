// Fill out your copyright notice in the Description page of Project Settings.


#include "TankPawn.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "TankController.h"
#include "Kismet/KismetMathLibrary.h"
#include "Cannon.h"
#include "Components/ArrowComponent.h"

DECLARE_LOG_CATEGORY_EXTERN(TankLog, All, All);
DEFINE_LOG_CATEGORY(TankLog);

ATankPawn::ATankPawn()
{
 	PrimaryActorTick.bCanEverTick = true;

	BodyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BodyMesh"));
	RootComponent = BodyMesh;

	TurretMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TurretMesh"));
	TurretMesh->SetupAttachment(BodyMesh);

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(BodyMesh);
	SpringArm->bDoCollisionTest = false;
	SpringArm->bInheritPitch = false;
	SpringArm->bInheritYaw = false;
	SpringArm->bInheritRoll = false;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);

	CannonSetupPoint = CreateDefaultSubobject<UArrowComponent>(TEXT("CannonSetupPoint"));
	CannonSetupPoint->SetupAttachment(TurretMesh);
}

void ATankPawn::MoveForward(float Value)
{
	TargetForwardAxisValue = Value;	
}

void ATankPawn::MoveRight(float Value)
{
	TargetRightAxisValue = Value;
}

void ATankPawn::RotateRight(float Value)
{
	RotateRightAxisValue = Value;
}

void ATankPawn::Fire()
{
	if (Cannon)
	{
		Cannon->Fire();
	}	
}

void ATankPawn::AlternateFire()
{
	if (AlternateCannon)
	{
		AlternateCannon->AlternateFire();
	}	
}

void ATankPawn::IncreaseAmmo(const int IncreaseAmmoValue)
{
	CurrentAmmo += IncreaseAmmoValue;
	if (CurrentAmmo > MaxAmmo)
	{
		CurrentAmmo = MaxAmmo;
	}
}

void ATankPawn::DecreaseAmmo(const int DecreaseAmmoValue)
{
	if (DecreaseAmmoValue > CurrentAmmo) return;
	CurrentAmmo -= DecreaseAmmoValue;
}

void ATankPawn::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	//Tank Movement
	FVector CurrentLocation = GetActorLocation();
	FVector ForwardVector = GetActorForwardVector();
	FVector RightVector = GetActorRightVector();
	FVector movePosition = CurrentLocation + (ForwardVector * MoveSpeed * TargetForwardAxisValue) + (RightVector * MoveSpeed * TargetRightAxisValue);
	SetActorLocation(movePosition, true);

	//Tank Rotation
	CurrentRotateAxisValue = FMath::Lerp(CurrentRotateAxisValue, RotateRightAxisValue, InterpolationKey);
	float YawRotation = RotateSpeed * CurrentRotateAxisValue * DeltaSeconds;

	//кUE_LOG(LogTemp, Warning, TEXT("CurrentRotateAxis Value: %f, RotateRightAxisValue: %f, "), CurrentRotateAxisValue, RotateRightAxisValue);
	
	FRotator CurrentRotation = GetActorRotation();
	YawRotation += CurrentRotation.Yaw;
	FRotator newRotation = FRotator(0.0f, YawRotation, 0.0f);
	SetActorRotation(newRotation);

	//Turret Rotation
	FVector MousePos = TankController->GetMousePosition();

	FRotator targetRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), MousePos);
	FRotator TurretRotation = TurretMesh->GetComponentRotation();

	targetRotation.Pitch = TurretRotation.Pitch;
	targetRotation.Roll = TurretRotation.Roll;

	TurretMesh->SetWorldRotation(FMath::Lerp(TurretRotation, targetRotation, RotateInterpolationKey));
}

void ATankPawn::BeginPlay()
{
	Super::BeginPlay();

	SetActorLocation(FVector(GetActorLocation().X, GetActorLocation().Y, 0.0f));

	TankController = Cast<ATankController>(GetController());

	SetupCannon(MainCannonClass);
}

void ATankPawn::SetupCannon(TSubclassOf<ACannon> newCannonClass)
{
	if(!newCannonClass)
	{
		return; 
	}
	if(Cannon)
	{
		Cannon->Destroy();
	}
		
	FActorSpawnParameters params;
	params.Instigator = this;
	params.Owner = this;
	
	Cannon = GetWorld()->SpawnActor<ACannon>(newCannonClass, params);
	Cannon->AttachToComponent(CannonSetupPoint, FAttachmentTransformRules::SnapToTargetIncludingScale);
	
	if(!AlternateCannonClass)
	{
		return; 
	}
	if(AlternateCannon)
	{
		AlternateCannon->Destroy();
	}
	
	AlternateCannon = GetWorld()->SpawnActor<ACannon>(AlternateCannonClass, params);
	AlternateCannon->AttachToComponent(CannonSetupPoint, FAttachmentTransformRules::SnapToTargetIncludingScale);
}

