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
		CurrentAmmo -= 3;
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, FString::Printf(TEXT("Ammo: %i"), CurrentAmmo));
		return;
	}
	CurrentShot++;
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, FString::Printf(TEXT("Burst Fire Projectile")));
	AProjectile* Projectile = GetWorld()->SpawnActor<AProjectile>(ProjectileClass, ProjectileSpawnPoint->GetComponentLocation(), ProjectileSpawnPoint->GetComponentRotation());
	if (Projectile)
	{
		Projectile->OnKilled.AddUObject(this, &ACannon::AddScore);
		Projectile->SetOwner(this);
		Projectile->Start();
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
		if(CurrentAmmo==0)
			{
			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, FString::Printf(TEXT("No Ammo For Projectile Fire")));
			return;
		}
		CurrentAmmo--;
		bMainCanFire = false;
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, FString::Printf(TEXT("Fire projectile. Ammo left: %i"), CurrentAmmo));
		if (ProjectilePool)
		{
			ProjectilePool->GetProjectile(ProjectileSpawnPoint->GetComponentLocation(), ProjectileSpawnPoint->GetComponentRotation());
		}
		else
		{
			AProjectile* Projectile = GetWorld()->SpawnActor<AProjectile>(ProjectileClass, ProjectileSpawnPoint->GetComponentLocation(), ProjectileSpawnPoint->GetComponentRotation());
			if (Projectile)
			{
				Projectile->OnKilled.AddUObject(this, &ACannon::AddScore);
				Projectile->SetOwner(this);
				Projectile->Start();
			}
		}
	}
	else if (CannonType==ECannonType::FireSPProjectile)
	{
		if(CurrentAmmo < 2)
		{
			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, FString::Printf(TEXT("No Ammo For SP-Projectile Fire")));
			return;
		}
		CurrentAmmo -=2;
		bMainCanFire = false;
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, FString::Printf(TEXT("Fire SP-projectile. Ammo left: %i"), CurrentAmmo));
		if (ProjectilePool)
		{
			ProjectilePool->GetProjectile(ProjectileSpawnPoint->GetComponentLocation(), ProjectileSpawnPoint->GetComponentRotation());
		}
		else
		{
			AProjectile* Projectile = GetWorld()->SpawnActor<AProjectile>(ProjectileClass, ProjectileSpawnPoint->GetComponentLocation(), ProjectileSpawnPoint->GetComponentRotation());
			if (Projectile)
			{
				Projectile->OnKilled.AddUObject(this, &ACannon::AddScore);
				Projectile->SetOwner(this);
				Projectile->Start();
			}
		}
	}
	else if (CannonType==ECannonType::FireTrace)
	{
		if(CurrentAmmo < 10)
		{
			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, FString::Printf(TEXT("No Ammo For Trace Fire")));
			return;
		}
		CurrentAmmo -= 10;
		bMainCanFire = false;
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, FString::Printf(TEXT("Fire trace! Ammo left: %i"), CurrentAmmo));

		FHitResult HitResult;
		FCollisionQueryParams TraceParams = FCollisionQueryParams(FName(TEXT("FireTrace")), true, this);
		TraceParams.bTraceComplex = true;
		TraceParams.bReturnPhysicalMaterial = false;

		FVector StartTrace = ProjectileSpawnPoint->GetComponentLocation();
		FVector EndTrace = StartTrace + ProjectileSpawnPoint->GetForwardVector() * FireRange;

		if (GetWorld()->LineTraceSingleByChannel(HitResult, StartTrace, EndTrace, ECollisionChannel::ECC_Visibility, TraceParams))
		{
			DrawDebugLine(GetWorld(), StartTrace, HitResult.Location, FColor::Red, false, 0.5f, 0, 5);
			if (HitResult.GetActor())
			{
				AActor* TraceOwner = GetOwner();
				AActor* OwnerByOwner = TraceOwner != nullptr ? TraceOwner->GetOwner() : nullptr;

				if (HitResult.GetActor() != TraceOwner && HitResult.GetActor() != OwnerByOwner)
				{
					if (IDamageTaker* DamageTakerActor = Cast<IDamageTaker>(HitResult.GetActor()))
					{
						FDamageData DamageData;
						DamageData.DamageValue = TraceDamage;
						DamageData.Instigator = TraceOwner;
						DamageData.DamageMaker = this;

						DamageTakerActor->TakeDamage(DamageData);
					}
				}
			}
			else
			{
				DrawDebugLine(GetWorld(), StartTrace, EndTrace, FColor::Purple, false, 0.5f, 0, 5);
			}
		}
	}
	else if (CannonType==ECannonType::FireMultiProjectile)
	{
		if(CurrentAmmo < 3)
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
		if(CurrentAmmo==0)
		{
			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, FString::Printf(TEXT("No Ammo For Projectile Fire")));
			return;
		}
		CurrentAmmo--;
		bAlternateCanFire = false;
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, FString::Printf(TEXT("Fire projectile. Ammo left: %i"), CurrentAmmo));
		if (ProjectilePool)
		{
			ProjectilePool->GetProjectile(ProjectileSpawnPoint->GetComponentLocation(), ProjectileSpawnPoint->GetComponentRotation());
		}
		else
		{
			AProjectile* Projectile = GetWorld()->SpawnActor<AProjectile>(ProjectileClass, ProjectileSpawnPoint->GetComponentLocation(), ProjectileSpawnPoint->GetComponentRotation());
			if (Projectile)
			{
				Projectile->OnKilled.AddUObject(this, &ACannon::AddScore);
				Projectile->SetOwner(this);
				Projectile->Start();
			}
		}
	}
	else if (CannonType==ECannonType::FireSPProjectile)
	{
		if(CurrentAmmo < 2)
		{
			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, FString::Printf(TEXT("No Ammo For SP-Projectile Fire")));
			return;
		}
		CurrentAmmo -= 2;
		bMainCanFire = false;
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, FString::Printf(TEXT("Fire SP-projectile. Ammo left: %i"), CurrentAmmo));
		if (ProjectilePool)
		{
			ProjectilePool->GetProjectile(ProjectileSpawnPoint->GetComponentLocation(), ProjectileSpawnPoint->GetComponentRotation());
		}
		else
		{
			AProjectile* Projectile = GetWorld()->SpawnActor<AProjectile>(ProjectileClass, ProjectileSpawnPoint->GetComponentLocation(), ProjectileSpawnPoint->GetComponentRotation());
			if (Projectile)
			{
				Projectile->OnKilled.AddUObject(this, &ACannon::AddScore);
				Projectile->SetOwner(this);
				Projectile->Start();
			}
		}
	}
	else if (CannonType==ECannonType::FireTrace)
	{
		if(CurrentAmmo < 10)
		{
			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, FString::Printf(TEXT("No Ammo For Trace Fire")));
			return;
		}
		CurrentAmmo -= 10;
		bMainCanFire = false;
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, FString::Printf(TEXT("Fire trace! Ammo left: %i"), CurrentAmmo));

		FHitResult HitResult;
		FCollisionQueryParams TraceParams = FCollisionQueryParams(FName(TEXT("FireTrace")), true, this);
		TraceParams.bTraceComplex = true;
		TraceParams.bReturnPhysicalMaterial = false;

		FVector StartTrace = ProjectileSpawnPoint->GetComponentLocation();
		FVector EndTrace = StartTrace + ProjectileSpawnPoint->GetForwardVector() * FireRange;

		if (GetWorld()->LineTraceSingleByChannel(HitResult, StartTrace, EndTrace, ECollisionChannel::ECC_Visibility, TraceParams))
		{
			DrawDebugLine(GetWorld(), StartTrace, HitResult.Location, FColor::Red, false, 0.5f, 0, 5);
			if (HitResult.GetActor())
			{
				AActor* TraceOwner = GetOwner();
				AActor* OwnerByOwner = TraceOwner != nullptr ? TraceOwner->GetOwner() : nullptr;

				if (HitResult.GetActor() != TraceOwner && HitResult.GetActor() != OwnerByOwner)
				{
					IDamageTaker* DamageTakerActor = Cast<IDamageTaker>(HitResult.GetActor());
					if (DamageTakerActor)
					{
						FDamageData damageData;
						damageData.DamageValue = TraceDamage;
						damageData.Instigator = TraceOwner;
						damageData.DamageMaker = this;

						DamageTakerActor->TakeDamage(damageData);
					}
				}
			}
			else
			{
				DrawDebugLine(GetWorld(), StartTrace, EndTrace, FColor::Purple, false, 0.5f, 0, 5);
			}
		}
	}
	else if (CannonType==ECannonType::FireMultiProjectile)
	{
		if(CurrentAmmo < 3)
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

void ACannon::IncreaseAmmoValue(const int AmmoValue)
{
	CurrentAmmo += AmmoValue;
	if (CurrentAmmo > MaxAmmo)
	{
		CurrentAmmo = MaxAmmo;
	}
}

void ACannon::AddScore(float ScoreValue)
{
	if (ScoreChanged.IsBound())
	{
		ScoreChanged.Broadcast(ScoreValue);
	}
}

void ACannon::BeginPlay()
{
	Super::BeginPlay();
	CreateProjectilePool();
	CurrentAmmo = MaxAmmo;
}
