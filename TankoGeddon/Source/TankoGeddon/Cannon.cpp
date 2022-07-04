// Fill out your copyright notice in the Description page of Project Settings.


#include "Cannon.h"
#include "Components/StaticMeshComponent.h"
#include "Components/ArrowComponent.h"

ACannon::ACannon()
{
 	PrimaryActorTick.bCanEverTick = false;

	CannonMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CannonMesh"));
	RootComponent = CannonMesh;

	ProjectileSpawnPoint = CreateDefaultSubobject<UArrowComponent>(TEXT("ProjectileSpawnPoint"));
	ProjectileSpawnPoint->SetupAttachment(CannonMesh);
}

void ACannon::BurstFire()
{
	if (CurrentShot==BurstFireRate)
	{
		GetWorld()->GetTimerManager().ClearTimer(BurstFireTimer);
		bMainCanFire = true;
		bAlternateCanFire = true;
		CurrentShot = 0;
		ProjectileAmmo--;
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, FString::Printf(TEXT("Projectile ammo: %d"), ProjectileAmmo));
		return;
	}
	CurrentShot++;
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, FString::Printf(TEXT("Fire Projectile")));
	GetWorld()->GetTimerManager().SetTimer(BurstFireTimer, this, &ACannon::BurstFire, BurstFireInterval, false);
}

void ACannon::Fire()
{
	if(!bMainCanFire)
	{
		return;
	}

	if(CannonType==ECannonType::FireProjectile)
	{
		if(ProjectileAmmo==0)
		{
			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, FString::Printf(TEXT("No projectile ammo")));
			return;
		}
		ProjectileAmmo--;
		bMainCanFire = false;
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, FString::Printf(TEXT("Fire projectile. Ammo left: %d"), ProjectileAmmo));
	}
	else if (CannonType==ECannonType::FireTrace)
	{
		if(TraceAmmo==0)
		{
			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, FString::Printf(TEXT("No Trace ammo")));
			return;
		}
		TraceAmmo--;
		bMainCanFire = false;
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, FString::Printf(TEXT("Fire trace! Ammo left: %d"), TraceAmmo));
	}
	else if (CannonType==ECannonType::FireMultiProjectile)
	{
		if(ProjectileAmmo==0)
		{
			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, FString::Printf(TEXT("No projectile ammo")));
			return;
		}
		BurstFire();
	}
	GetWorld()->GetTimerManager().SetTimer(ReloadTimer, this, &ACannon::MainReload, ReloadTime, false);
}

void ACannon::MainReload()
{
	bMainCanFire = true;
}

void ACannon::AlternateFire()
{
	if(!bAlternateCanFire)
	{
		return;
	}

	if(CannonType==ECannonType::FireProjectile)
	{
		if(ProjectileAmmo==0)
		{
			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, FString::Printf(TEXT("No projectile ammo")));
			return;
		}
		ProjectileAmmo--;
		bAlternateCanFire = false;
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, FString::Printf(TEXT("Fire projectile. Ammo left: %d"), ProjectileAmmo));
	}
	else if (CannonType==ECannonType::FireTrace)
	{
		if(TraceAmmo==0)
		{
			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, FString::Printf(TEXT("No Trace ammo")));
			return;
		}
		TraceAmmo--;
		bAlternateCanFire = false;
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, FString::Printf(TEXT("Fire trace! Ammo left: %d"), TraceAmmo));
	}
	else if (CannonType==ECannonType::FireMultiProjectile)
	{
		if(ProjectileAmmo==0)
		{
			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, FString::Printf(TEXT("No projectile ammo")));
			return;
		}
		BurstFire();
	}
	GetWorld()->GetTimerManager().SetTimer(ReloadTimer, this, &ACannon::AlternateReload, ReloadTime, false);
}

void ACannon::AlternateReload()
{
	bAlternateCanFire = true;
}
