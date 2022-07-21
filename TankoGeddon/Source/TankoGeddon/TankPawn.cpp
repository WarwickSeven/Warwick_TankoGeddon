// Fill out your copyright notice in the Description page of Project Settings.


#include "TankPawn.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "TankController.h"
#include "Kismet/KismetMathLibrary.h"

DECLARE_LOG_CATEGORY_EXTERN(LogTank, All, All);
DEFINE_LOG_CATEGORY(LogTank);

ATankPawn::ATankPawn()
{
 	PrimaryActorTick.bCanEverTick = true;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(BodyMesh);
	SpringArm->bDoCollisionTest = false;
	SpringArm->bInheritPitch = false;
	SpringArm->bInheritYaw = false;
	SpringArm->bInheritRoll = false;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);
}

void ATankPawn::MoveForward(const float Value)
{
	TargetForwardAxisValue = Value;	
}

void ATankPawn::MoveRight(const float Value)
{
	TargetRightAxisValue = Value;
}

void ATankPawn::RotateRight(const float Value)
{
	RotateRightAxisValue = Value;
}

void ATankPawn::Tick(const float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	//Tank Movement
	const FVector CurrentLocation = GetActorLocation();
	const FVector ForwardVector = GetActorForwardVector();
	const FVector RightVector = GetActorRightVector();
	const FVector MovePosition = CurrentLocation + (ForwardVector * MoveSpeed * TargetForwardAxisValue) + (RightVector * MoveSpeed * TargetRightAxisValue);
	SetActorLocation(MovePosition, true);

	//Tank Rotation
	CurrentRotateAxisValue = FMath::Lerp(CurrentRotateAxisValue, RotateRightAxisValue, InterpolationKey);
	float YawRotation = RotateSpeed * CurrentRotateAxisValue * DeltaSeconds;

	//UE_LOG(LogTemp, Warning, TEXT("CurrentRotateAxis Value: %f, RotateRightAxisValue: %f, "), CurrentRotateAxisValue, RotateRightAxisValue);

	const FRotator CurrentRotation = GetActorRotation();
	YawRotation += CurrentRotation.Yaw;
	const FRotator NewRotation = FRotator(0.0f, YawRotation, 0.0f);
	SetActorRotation(NewRotation);

	//Turret Rotation
	if (TankController)
	{
		const FVector MousePos = TankController->GetMousePosition();

		FRotator TargetRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), MousePos);
		const FRotator TurretRotation = TurretMesh->GetComponentRotation();

		TargetRotation.Pitch = TurretRotation.Pitch;
		TargetRotation.Roll = TurretRotation.Roll;

		TurretMesh->SetWorldRotation(FMath::Lerp(TurretRotation, TargetRotation, RotateInterpolationKey));
	}
}

void ATankPawn::BeginPlay()
{
	Super::BeginPlay();

	SetActorLocation(FVector(GetActorLocation().X, GetActorLocation().Y, 0.0f));

	TankController = Cast<ATankController>(GetController());
}

