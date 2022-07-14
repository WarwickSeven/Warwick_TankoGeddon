// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NewCannonBox.generated.h"

UCLASS()
class TANKOGEDDON_API ANewCannonBox : public AActor
{
	GENERATED_BODY()
	
public:	
	ANewCannonBox();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Component")
	class UStaticMeshComponent* AmmoMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Component")
	TSubclassOf<class ACannon> CannonClass;

	UFUNCTION()
	void OnMeshOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const
		FHitResult& SweepResult);
};