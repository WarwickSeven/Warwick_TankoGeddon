// Fill out your copyright notice in the Description page of Project Settings.


#include "Turret.h"
#include "Cannon.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "UObject/UObjectGlobals.h"
#include "Engine/StaticMesh.h"
#include "TimerManager.h"

ATurret::ATurret()
{
	PrimaryActorTick.bCanEverTick = false;

	if(UStaticMesh* BodyMeshTemp = LoadObject<UStaticMesh>(this, *BodyMeshPath))
		BodyMesh->SetStaticMesh(BodyMeshTemp);

	if(UStaticMesh* TurretMeshTemp = LoadObject<UStaticMesh>(this, *TurretMeshPath))
		TurretMesh->SetStaticMesh(TurretMeshTemp);
}

void ATurret::BeginPlay()
{
	Super::BeginPlay();
	
	PlayerPawn = GetWorld()->GetFirstPlayerController()->GetPawn();

	FTimerHandle TargetingTimer;
	GetWorld()->GetTimerManager().SetTimer(TargetingTimer,this, &ATurret::Targeting, TargetingRate,true, TargetingRate);
}

void ATurret::Targeting() const
{
	if (IsPlayerInRange())
	{
		RotateToPlayer();

		if (CanFire() && Cannon && Cannon->IsReadyToFire())
		{
			Fire();
		}
	}
}

void ATurret::RotateToPlayer() const
{
	FRotator TargetRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), PlayerPawn->GetActorLocation());
	const FRotator TurretRotation = TurretMesh->GetComponentRotation();
	TargetRotation.Pitch = TurretRotation.Pitch;
	TargetRotation.Roll = TurretRotation.Roll;
	TurretMesh->SetWorldRotation(FMath::Lerp(TurretRotation, TargetRotation, TargetingSpeed));
}

bool ATurret::IsPlayerInRange() const
{
	return FVector::Distance(PlayerPawn->GetActorLocation(), GetActorLocation()) <= TargetingRange;	
}

bool ATurret::CanFire() const
{
	const FVector TargetingDir = TurretMesh->GetForwardVector();
	FVector DirToPlayer = PlayerPawn->GetActorLocation() - GetActorLocation();
	DirToPlayer.Normalize();

	const float AimAngle = FMath::RadiansToDegrees(acos(FVector::DotProduct(TargetingDir, DirToPlayer)));
	return AimAngle <= Accurency; 
}
