// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProjectilePool.generated.h"

class AProjectile;
UCLASS()
class TANKOGEDDON_API AProjectilePool final : public AActor
{
	GENERATED_BODY()
	
public:	
	void GetProjectile(FVector SpawnLocation, FRotator SpawnRotation);

protected:
	virtual void BeginPlay() override;

	void SpawnProjectile(FVector SpawnLocation, FRotator SpawnRotation, bool bIsActive = false );

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Options")
	int32 PoolSize = 6;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Options")
	TSubclassOf<AProjectile> ProjectileClass;

	TArray<AProjectile*> ProjectilePool;

private:
	void InitializePool();

	bool bHaveActiveProjectile = false;
};