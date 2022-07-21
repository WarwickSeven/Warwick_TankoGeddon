// Fill out your copyright notice in the Description page of Project Settings.

#include "ProjectilePool.h"
#include "Projectile.h"

void AProjectilePool::GetProjectile(const FVector SpawnLocation, const FRotator SpawnRotation)
{
	bHaveActiveProjectile = false;
	for (int32 i = 0; i < ProjectilePool.Num(); i++)
	{
		if (ProjectilePool[i]->bIsActivation == false)
		{
			ProjectilePool[i]->SetActorLocation(SpawnLocation);
			ProjectilePool[i]->SetActorRotation(SpawnRotation);
			ProjectilePool[i]->bIsActivation = true;
			bHaveActiveProjectile = true;
			ProjectilePool[i]->SetActorEnableCollision(true);
			ProjectilePool[i]->Start();
			return;
		}
	}
	if (bHaveActiveProjectile == false)
	{
		SpawnProjectile(SpawnLocation, SpawnRotation, true);
	}
}

void AProjectilePool::BeginPlay()
{
	Super::BeginPlay();
	InitializePool();
}

void AProjectilePool::SpawnProjectile(const FVector SpawnLocation, const FRotator SpawnRotation, const bool bIsActive)
{
	if (AProjectile* Projectile = GetWorld()->SpawnActor<AProjectile>(ProjectileClass, SpawnLocation, SpawnRotation))
	{
		ProjectilePool.Add(Projectile);
		Projectile->bIsActivation = bIsActive;
		Projectile->SetActorEnableCollision(bIsActive);

		if (bIsActive)
		{
			Projectile->Start();
		}
	}
}

void AProjectilePool::InitializePool()
{
	for (int32 i = 0; i < PoolSize; i++)
	{
		SpawnProjectile(FVector(0.0f, 0.0f, -500.0f), FRotator(0.0f, 0.0f, 0.0f));
	}
}