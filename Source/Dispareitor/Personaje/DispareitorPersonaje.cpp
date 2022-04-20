// Fill out your copyright notice in the Description page of Project Settings.


#include "DispareitorPersonaje.h"

// Sets default values
ADispareitorPersonaje::ADispareitorPersonaje()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ADispareitorPersonaje::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ADispareitorPersonaje::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ADispareitorPersonaje::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

