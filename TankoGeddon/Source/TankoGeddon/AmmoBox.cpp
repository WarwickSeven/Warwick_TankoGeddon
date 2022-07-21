// Fill out your copyright notice in the Description page of Project Settings.


#include "AmmoBox.h"

AAmmoBox::AAmmoBox()
{
	USceneComponent* AmmoSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	RootComponent = AmmoSceneComponent;

	AmmoMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("AmmoMesh"));
	AmmoMesh->SetupAttachment(AmmoSceneComponent);
	AmmoMesh->OnComponentBeginOverlap.AddDynamic(this, &AAmmoBox::OnMeshOverlapBegin);
	AmmoMesh->SetCollisionProfileName(FName("OverlapAll"));
	AmmoMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	AmmoMesh->SetGenerateOverlapEvents(true);
}

void AAmmoBox::OnMeshOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	const ATankPawn* TankPawn = Cast<ATankPawn>(OtherActor);
	
	if(TankPawn && Cannon)
	{
		if (Cannon->GetCurrentAmmoValue() == Cannon->GetMaxAmmoValue())
		{
			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, FString::Printf(TEXT("Can't pick, Full Ammo: %d"), Cannon->GetCurrentAmmoValue()));	
			return;
		}
		Cannon->IncreaseAmmoValue(AmmoBoxVolume);
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, FString::Printf(TEXT("Current Ammo: %d"), Cannon->GetCurrentAmmoValue()));
		Destroy();
	}
}