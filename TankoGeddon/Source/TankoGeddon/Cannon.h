// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameStruct.h"
#include "TankPawn.h"
#include "Cannon.generated.h"

class AProjectilePool;
UCLASS()
class TANKOGEDDON_API ACannon : public AActor
{
	GENERATED_BODY()
	
public:	
	ACannon();
	void Fire();
	void AlternateFire();
	void MainReload();
	void AlternateReload();

	bool IsReadyToFire() const { return bMainCanFire; };	
	void CreateProjectilePool();
protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	class UStaticMeshComponent* CannonMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	class UArrowComponent* ProjectileSpawnPoint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	ECannonType CannonType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	float ReloadTime = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	int BurstFireRate = 3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	float BurstFireInterval = 0.2f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	float FireRange = 1000;
	
	FTimerHandle ReloadTimer;
	FTimerHandle BurstFireTimer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Projectile")
	TSubclassOf<class AProjectile> ProjectileClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	TSubclassOf<AProjectilePool> ProjectilePoolClass;

	UPROPERTY()
	AProjectilePool* ProjectilePool;
	
	UPROPERTY()
	ATankPawn* TankPawn;
	
private:
	bool bMainCanFire = true;
	bool bAlternateCanFire = true;
	int CurrentShot;
	void BurstFire();
};
