// Fill out your copyright notice in the Description page of Project Settings.


#include "AmmoBox.h"

#include "TankPawn.h"

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
	ATankPawn* TankPawn = Cast<ATankPawn>(OtherActor);

	if(TankPawn)
	{
		if (TankPawn->GetAmmoValue() == TankPawn->GetMaxAmmoValue())
		{
			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, FString::Printf(TEXT("Can't pick, Full Ammo: %d"), TankPawn->GetAmmoValue()));	
			return;
		}
		TankPawn->IncreaseAmmo(AmmoBoxVolume);
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, FString::Printf(TEXT("Current Ammo: %d"), TankPawn->GetAmmoValue()));
		Destroy();
	}
}