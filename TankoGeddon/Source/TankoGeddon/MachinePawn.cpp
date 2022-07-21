// Fill out your copyright notice in the Description page of Project Settings.


#include "MachinePawn.h"
#include "Cannon.h"
#include "HealthComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"

AMachinePawn::AMachinePawn()
{
	BodyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BodyMesh"));
	RootComponent = BodyMesh;

	TurretMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TurretMesh"));
	TurretMesh->SetupAttachment(BodyMesh);

	CannonSetupPoint = CreateDefaultSubobject<UArrowComponent>(TEXT("CannonSetupPoint"));
	CannonSetupPoint->SetupAttachment(TurretMesh);

	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollider"));
	BoxComponent->SetupAttachment(BodyMesh);

	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));
	HealthComponent->OnDie.AddUObject(this, &AMachinePawn::Die);
	HealthComponent->OnHealthChanged.AddUObject(this, &AMachinePawn::DamageTaked);
}

void AMachinePawn::TakeDamage(const FDamageData DamageData)
{
	HealthComponent->TakeDamage(DamageData);
}

void AMachinePawn::SetupCannon(const TSubclassOf<ACannon> NewCannonClass)
{
	if(!NewCannonClass)
	{
		return; 
	}
	if(Cannon)
	{
		Cannon->Destroy();
	}
		
	FActorSpawnParameters Params;
	Params.Instigator = this;
	Params.Owner = this;
	
	Cannon = GetWorld()->SpawnActor<ACannon>(NewCannonClass, Params);
	Cannon->SetOwner(this);
	Cannon->ScoreChanged.AddUObject(this, &AMachinePawn::ShowScore);
	
	Cannon->AttachToComponent(CannonSetupPoint, FAttachmentTransformRules::SnapToTargetIncludingScale);
	
	if(!AlternateCannonClass)
	{
		return; 
	}
	if(AlternateCannon)
	{
		AlternateCannon->Destroy();
	}
	
	AlternateCannon = GetWorld()->SpawnActor<ACannon>(AlternateCannonClass, Params);
	AlternateCannon->AttachToComponent(CannonSetupPoint, FAttachmentTransformRules::SnapToTargetIncludingScale);
}

void AMachinePawn::Fire() const
{
	if (Cannon)
		Cannon->Fire();
}

void AMachinePawn::AlternateFire() const
{
	if (AlternateCannon)
	{
		AlternateCannon->AlternateFire();
	}	
}

float AMachinePawn::GetPoints()
{
	return ScoreValue;
}

void AMachinePawn::ShowScore(const float Value)
{
	Score += Value;
	GEngine->AddOnScreenDebugMessage(-1, 0.2f, FColor::Green, FString::Printf(TEXT("Score: %f"), Score));
}

void AMachinePawn::BeginPlay()
{
	Super::BeginPlay();
	SetupCannon(CannonClass);
}

void AMachinePawn::Die()
{
	if (Cannon)
	{
		Cannon->Destroy();
	}
	Destroy();
}

void AMachinePawn::DamageTaked(const float DamageValue) const
{
	UE_LOG(LogTemp, Warning, TEXT("Turret %s take Damage: %f, Health: %f"), *GetName(), DamageValue, HealthComponent->GetHealth());
}
