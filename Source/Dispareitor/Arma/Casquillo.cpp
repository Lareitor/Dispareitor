#include "Casquillo.h"

ACasquillo::ACasquillo() {
	PrimaryActorTick.bCanEverTick = false;

	Malla = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Malla"));
	SetRootComponent(Malla);
}

void ACasquillo::BeginPlay() {
	Super::BeginPlay();
}

