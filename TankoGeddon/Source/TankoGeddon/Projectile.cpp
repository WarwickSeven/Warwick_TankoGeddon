// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile.h"
#include "DamageTaker.h"
#include "Scorable.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
//#include "GameStruct.h"

AProjectile::AProjectile()
{
 	PrimaryActorTick.bCanEverTick = false;

	USceneComponent* SceneComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RootComponent"));
	RootComponent = SceneComponent;
	
	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Projectile"));
	ProjectileMesh->SetupAttachment(SceneComponent);
	ProjectileMesh->OnComponentBeginOverlap.AddDynamic(this, &AProjectile::OnMeshOverlapBegin);
	ProjectileMesh->SetCollisionObjectType(ECollisionChannel::ECC_GameTraceChannel1);
}

void AProjectile::Start()
{
	GetWorld()->GetTimerManager().SetTimer(MoveTimer, this, &AProjectile::Move, MoveRate, true, MoveRate);
	GetWorld()->GetTimerManager().SetTimer(DeactivateTimer, this, &AProjectile::Deactivate, DeactivateTime, false);
}

void AProjectile::Deactivate()
{
	bIsActivation = false;
	SetActorEnableCollision(false);
	SetActorLocation(FVector(0.0f, 0.0f, -500.0f));
	GetWorld()->GetTimerManager().ClearTimer(DeactivateTimer);
	GetWorld()->GetTimerManager().ClearTimer(MoveTimer);
}

void AProjectile::Move()
{
	const FVector NextPosition = GetActorLocation() + GetActorForwardVector() * MoveSpeed * MoveRate;
	SetActorLocation(NextPosition);
}

void AProjectile::OnMeshOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("Projectile collided with %s, collided with component %s"), *OtherActor->GetName(), *OverlappedComp->GetName());

	const AActor* ProjectileOwner = GetOwner();
	const AActor* OwnerByOwner = ProjectileOwner != nullptr ? ProjectileOwner->GetOwner() : nullptr; 

	if (OtherActor != ProjectileOwner && OtherActor != OwnerByOwner)
	{
		IDamageTaker* DamageTakerActor = Cast<IDamageTaker>(OtherActor);
		IScorable* ScorableActor = Cast<IScorable>(OtherActor);

		float ScoreValue = 0.0f;

		if(ScorableActor)
		{
			ScoreValue = ScorableActor->GetPoints();	
		}
		
		if (DamageTakerActor)
		{
			FDamageData DamageData;
			DamageData.DamageValue = Damage;
			DamageData.Instigator = Owner;
			DamageData.DamageMaker = this;

			DamageTakerActor->TakeDamage(DamageData);

			if (OtherActor->IsActorBeingDestroyed() && ScoreValue != 0.0f)
			{
				if (OnKilled.IsBound())
				{
					OnKilled.Broadcast(ScoreValue);
				}
			}
		}
	}
	Deactivate();
} 