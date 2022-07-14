// Fill out your copyright notice in the Description page of Project Settings.

#include "Cannon.h"
#include "Components/StaticMeshComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/SceneComponent.h"
#include "Projectile.h"
#include "DrawDebugHelpers.h"
#include "ProjectilePool.h"
#include "TankPawn.h"
#include "Particles/ParticleSystemComponent.h"

ACannon::ACannon()
{
	PrimaryActorTick.bCanEverTick = false;

	USceneComponent* CannonSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("CannonRoot"));
	RootComponent = CannonSceneComponent;

	CannonMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CannonMesh"));
	CannonMesh->SetupAttachment(CannonSceneComponent);

	ProjectileSpawnPoint = CreateDefaultSubobject<UArrowComponent>(TEXT("ProjectileSpawnPoint"));
	ProjectileSpawnPoint->SetupAttachment(CannonSceneComponent);
}

void ACannon::BurstFire()
{
	if (CurrentShot==BurstFireRate)
	{
		GetWorld()->GetTimerManager().ClearTimer(BurstFireTimer);
		bMainCanFire = true;
		bAlternateCanFire = true;
		CurrentShot = 0;
		TankPawn->DecreaseAmmo(3);
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, FString::Printf(TEXT("Ammo: %d"), TankPawn->GetAmmoValue()));
		return;
	}
	CurrentShot++;
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, FString::Printf(TEXT("Burst Fire Projectile")));
	AProjectile* projectile = GetWorld()->SpawnActor<AProjectile>(ProjectileClass, ProjectileSpawnPoint->GetComponentLocation(), ProjectileSpawnPoint->GetComponentRotation());
	if (projectile)
	{
		projectile->Start();
	}
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
		if(TankPawn->GetAmmoValue()==0)
		{
			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, FString::Printf(TEXT("No Ammo For Projectile Fire")));
			return;
		}
		TankPawn->DecreaseAmmo(1);
		bMainCanFire = false;
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, FString::Printf(TEXT("Fire projectile. Ammo left: %d"), TankPawn->GetAmmoValue()));
		if (ProjectilePool)
		{
			ProjectilePool->GetProjectile(ProjectileSpawnPoint->GetComponentLocation(), ProjectileSpawnPoint->GetComponentRotation());
		}
		else
		{
			AProjectile* projectile = GetWorld()->SpawnActor<AProjectile>(ProjectileClass, ProjectileSpawnPoint->GetComponentLocation(), ProjectileSpawnPoint->GetComponentRotation());
			if (projectile)
			{
				projectile->Start();
			}
		}
	}
	else if (CannonType==ECannonType::FireSPProjectile)
	{
		if(TankPawn->GetAmmoValue() < 2)
		{
			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, FString::Printf(TEXT("No Ammo For SP-Projectile Fire")));
			return;
		}
		TankPawn->DecreaseAmmo(2);
		bMainCanFire = false;
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, FString::Printf(TEXT("Fire SP-projectile. Ammo left: %d"), TankPawn->GetAmmoValue()));
		if (ProjectilePool)
		{
			ProjectilePool->GetProjectile(ProjectileSpawnPoint->GetComponentLocation(), ProjectileSpawnPoint->GetComponentRotation());
		}
		else
		{
			AProjectile* projectile = GetWorld()->SpawnActor<AProjectile>(ProjectileClass, ProjectileSpawnPoint->GetComponentLocation(), ProjectileSpawnPoint->GetComponentRotation());
			if (projectile)
			{
				projectile->Start();
			}
		}
	}
	else if (CannonType==ECannonType::FireTrace)
	{
		if(TankPawn->GetAmmoValue() < 10)
		{
			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, FString::Printf(TEXT("No Ammo For Trace Fire")));
			return;
		}
		TankPawn->DecreaseAmmo(10);
		bMainCanFire = false;
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, FString::Printf(TEXT("Fire trace! Ammo left: %d"), TankPawn->GetAmmoValue()));

		FHitResult hitResult;
		FCollisionQueryParams traceParams = FCollisionQueryParams(FName(TEXT("FireTrace")), true, this);
		traceParams.bTraceComplex = true;
		traceParams.bReturnPhysicalMaterial = false;

		FVector StartTrace = ProjectileSpawnPoint->GetComponentLocation();
		FVector EndTrace = StartTrace + ProjectileSpawnPoint->GetForwardVector() * FireRange;

		if (GetWorld()->LineTraceSingleByChannel(hitResult, StartTrace, EndTrace, ECollisionChannel::ECC_Visibility, traceParams))
		{
			DrawDebugLine(GetWorld(), StartTrace, hitResult.Location, FColor::Red, false, 0.5f, 0, 5);
			if (hitResult.GetActor())
			{
				hitResult.GetActor()->Destroy();
			}
		}
		else
		{
			DrawDebugLine(GetWorld(), StartTrace, EndTrace, FColor::Purple, false, 0.5f, 0, 5);
		}
	}
	else if (CannonType==ECannonType::FireMultiProjectile)
	{
		if(TankPawn->GetAmmoValue() < 3)
		{
			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, FString::Printf(TEXT("No Ammo For Burst Fire")));
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
		if(TankPawn->GetAmmoValue()==0)
		{
			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, FString::Printf(TEXT("No Ammo For Projectile Fire")));
			return;
		}
		TankPawn->DecreaseAmmo(1);
		bAlternateCanFire = false;
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, FString::Printf(TEXT("Fire projectile. Ammo left: %d"), TankPawn->GetAmmoValue()));
		if (ProjectilePool)
		{
			ProjectilePool->GetProjectile(ProjectileSpawnPoint->GetComponentLocation(), ProjectileSpawnPoint->GetComponentRotation());
		}
		else
		{
			AProjectile* projectile = GetWorld()->SpawnActor<AProjectile>(ProjectileClass, ProjectileSpawnPoint->GetComponentLocation(), ProjectileSpawnPoint->GetComponentRotation());
			if (projectile)
			{
				projectile->Start();
			}
		}
	}
	else if (CannonType==ECannonType::FireSPProjectile)
	{
		if(TankPawn->GetAmmoValue() < 2)
		{
			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, FString::Printf(TEXT("No Ammo For SP-Projectile Fire")));
			return;
		}
		TankPawn->DecreaseAmmo(2);
		bMainCanFire = false;
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, FString::Printf(TEXT("Fire SP-projectile. Ammo left: %d"), TankPawn->GetAmmoValue()));
		if (ProjectilePool)
		{
			ProjectilePool->GetProjectile(ProjectileSpawnPoint->GetComponentLocation(), ProjectileSpawnPoint->GetComponentRotation());
		}
		else
		{
			AProjectile* projectile = GetWorld()->SpawnActor<AProjectile>(ProjectileClass, ProjectileSpawnPoint->GetComponentLocation(), ProjectileSpawnPoint->GetComponentRotation());
			if (projectile)
			{
				projectile->Start();
			}
		}
	}
	else if (CannonType==ECannonType::FireTrace)
	{
		if(TankPawn->GetAmmoValue() < 10)
		{
			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, FString::Printf(TEXT("No Ammo For Trace Fire")));
			return;
		}
		TankPawn->DecreaseAmmo(10);
		bAlternateCanFire = false;
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, FString::Printf(TEXT("Fire trace! Ammo left: %d"), TankPawn->GetAmmoValue()));

		FHitResult hitResult;
		FCollisionQueryParams traceParams = FCollisionQueryParams(FName(TEXT("FireTrace")), true, this);
		traceParams.bTraceComplex = true;
		traceParams.bReturnPhysicalMaterial = false;

		FVector StartTrace = ProjectileSpawnPoint->GetComponentLocation();
		FVector EndTrace = StartTrace + ProjectileSpawnPoint->GetForwardVector() * FireRange;

		if (GetWorld()->LineTraceSingleByChannel(hitResult, StartTrace, EndTrace, ECollisionChannel::ECC_Visibility, traceParams))
		{
			DrawDebugLine(GetWorld(), StartTrace, hitResult.Location, FColor::Red, false, 0.5f, 0, 5);
			if (hitResult.GetActor())
			{
				hitResult.GetActor()->Destroy();
			}
		}
		else
		{
			DrawDebugLine(GetWorld(), StartTrace, EndTrace, FColor::Purple, false, 0.5f, 0, 5);
		}
	}
	else if (CannonType==ECannonType::FireMultiProjectile)
	{
		if(TankPawn->GetAmmoValue() < 3)
		{
			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, FString::Printf(TEXT("No Ammo For Burst Fire")));
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

void ACannon::CreateProjectilePool()
{
	if(ProjectilePoolClass)
		ProjectilePool = GetWorld()->SpawnActor<AProjectilePool>(ProjectilePoolClass, ProjectileSpawnPoint->GetComponentLocation(), ProjectileSpawnPoint->GetComponentRotation());
}

void ACannon::BeginPlay()
{
	Super::BeginPlay();
	CreateProjectilePool();
	TankPawn = Cast<ATankPawn>(GetOwner());
}


