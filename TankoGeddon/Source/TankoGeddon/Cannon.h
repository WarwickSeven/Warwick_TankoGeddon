// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameStruct.h"
#include "GameFramework/Actor.h"
#include "Cannon.generated.h"

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
protected:
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
	int ProjectileAmmo = 5;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	int TraceAmmo = 3;
	
	FTimerHandle ReloadTimer;
	FTimerHandle BurstFireTimer;
private:
	bool bMainCanFire = true;
	bool bAlternateCanFire = true;
	int CurrentShot;
	void BurstFire();
};
