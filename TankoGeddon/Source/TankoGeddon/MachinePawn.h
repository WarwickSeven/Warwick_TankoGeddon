// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DamageTaker.h"
#include "GameStruct.h"
#include "Scorable.h"
#include "GameFramework/Pawn.h"
#include "MachinePawn.generated.h"

class UStaticMeshComponent;
class ACannon; 

UCLASS()
class TANKOGEDDON_API AMachinePawn : public APawn, public IDamageTaker, public IScorable
{
	GENERATED_BODY()

public:
	AMachinePawn();
	virtual void TakeDamage(FDamageData DamageData) override;
	void SetupCannon(TSubclassOf<ACannon> NewCannonClass);
	void Fire() const;
	void AlternateFire() const;

	//Scorable
	virtual float GetPoints() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Scorable")
	float ScoreValue = 0.0f;

	UFUNCTION()
	void ShowScore(float Value);

	float Score = 0.0f;
	
protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Components")
	UStaticMeshComponent* BodyMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Components")
	UStaticMeshComponent* TurretMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Components")
	class UBoxComponent* BoxComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Components")
	class UHealthComponent* HealthComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Components")
	class UArrowComponent* CannonSetupPoint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Turret | Components")
	TSubclassOf<ACannon> CannonClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Turret | Components")
	TSubclassOf<ACannon> AlternateCannonClass;

	UPROPERTY()
	ACannon* Cannon;

	UPROPERTY()
	ACannon* AlternateCannon;

	UFUNCTION()
	void Die();

	UFUNCTION()
	void DamageTaked(float DamageValue) const;
};
