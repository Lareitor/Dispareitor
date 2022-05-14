#include "Proyectil.h"
#include "Components/BoxComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Particles/ParticleSystem.h"

AProyectil::AProyectil() {
	PrimaryActorTick.bCanEverTick = true;
	// El proyectil es creado en el servidor pero como tenemos esta variable a true, es replicada en los clientes tambien
	bReplicates = true;

	CajaColision = CreateDefaultSubobject<UBoxComponent>(TEXT("CajaColision"));
	SetRootComponent(CajaColision);
	CajaColision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	CajaColision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CajaColision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	CajaColision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	CajaColision->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);

	ProyectilMovimientoComponente = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProyectilMovimientoComponente"));
	ProyectilMovimientoComponente->bRotationFollowsVelocity = true;
}

void AProyectil::BeginPlay() {
	Super::BeginPlay();

	if(Traza) {
		TrazaComponente = UGameplayStatics::SpawnEmitterAttached(Traza, CajaColision, FName(), GetActorLocation(), GetActorRotation(), EAttachLocation::KeepWorldPosition);
	}
}

void AProyectil::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}

