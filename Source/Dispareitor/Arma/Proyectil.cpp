#include "Proyectil.h"
#include "Components/BoxComponent.h"

AProyectil::AProyectil() {
	PrimaryActorTick.bCanEverTick = true;

	CajaColision = CreateDefaultSubobject<UBoxComponent>(TEXT("CajaColision"));
	SetRootComponent(CajaColision);
	CajaColision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	CajaColision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CajaColision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	CajaColision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	CajaColision->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);
}

void AProyectil::BeginPlay() {
	Super::BeginPlay();
}

void AProyectil::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}

