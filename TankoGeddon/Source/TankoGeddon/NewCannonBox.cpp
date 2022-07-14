// Fill out your copyright notice in the Description page of Project Settings.


#include "NewCannonBox.h"
#include "TankPawn.h"

ANewCannonBox::ANewCannonBox()
{
	USceneComponent* AmmoSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	RootComponent = AmmoSceneComponent;

	AmmoMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("AmmoMesh"));
	AmmoMesh->SetupAttachment(AmmoSceneComponent);
	AmmoMesh->OnComponentBeginOverlap.AddDynamic(this, &ANewCannonBox::OnMeshOverlapBegin);
	AmmoMesh->SetCollisionProfileName(FName("OverlapAll"));
	AmmoMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	AmmoMesh->SetGenerateOverlapEvents(true);
}

void ANewCannonBox::OnMeshOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(ATankPawn* TankPawn = Cast<ATankPawn>(OtherActor))
	{
		TankPawn->SetupCannon(CannonClass);
		Destroy();
	}
}