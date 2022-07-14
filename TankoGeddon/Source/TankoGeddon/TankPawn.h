// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "GameStruct.h"
#include "TankPawn.generated.h"


class UStaticMeshComponent;
class ACannon;
UCLASS()
class TANKOGEDDON_API ATankPawn : public APawn
{
	GENERATED_BODY()
	
public:
	ATankPawn();
	
	void MoveForward(float Value);
	void MoveRight(float Value);
	void RotateRight(float Value);

	void SetupCannon(TSubclassOf<ACannon> newCannonClass);
	void Fire();
	void AlternateFire();

	void IncreaseAmmo(int IncreaseAmmoValue);
	void DecreaseAmmo(int DecreaseAmmoValue);
	int GetAmmoValue() const { return CurrentAmmo; }
	int GetMaxAmmoValue() const { return MaxAmmo; }
	
	virtual void Tick(float DeltaSeconds) override;
	virtual void BeginPlay() override;
protected:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category="Components")
	UStaticMeshComponent* BodyMesh;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category="Components")
	UStaticMeshComponent* TurretMesh;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category="Components")
	class USpringArmComponent* SpringArm;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category="Components")
	class UCameraComponent* Camera;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Turret | Components")
	class UArrowComponent* CannonSetupPoint;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Turret | Components")
	TSubclassOf<ACannon> MainCannonClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Turret | Components")
	TSubclassOf<ACannon> AlternateCannonClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Turret | Components")
	int CurrentAmmo = 120;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Turret | Components")
	int MaxAmmo = 120;

	UPROPERTY()
	ACannon* Cannon;

	UPROPERTY()
	ACannon* AlternateCannon;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Movement")
	float MoveSpeed = 10.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Movement")
	float RotateSpeed = 100.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Movement")
	float InterpolationKey = 0.1f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Movement")
	float RotateInterpolationKey = 0.1f;

	UPROPERTY()
	class ATankController* TankController;

private:	
	float TargetForwardAxisValue = 0.0f;
	float TargetRightAxisValue = 0.0f;
	float RotateRightAxisValue = 0.0f;
	float CurrentRotateAxisValue = 0.0f;
};