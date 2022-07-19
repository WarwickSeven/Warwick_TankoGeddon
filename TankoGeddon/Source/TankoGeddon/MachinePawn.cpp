// Fill out your copyright notice in the Description page of Project Settings.


#include "MachinePawn.h"

// Sets default values
AMachinePawn::AMachinePawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AMachinePawn::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMachinePawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AMachinePawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

