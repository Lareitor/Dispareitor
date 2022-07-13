#include "ProyectilGranada.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "GameFramework/ProjectileMovementComponent.h"

AProyectilGranada::AProyectilGranada() {
    Malla = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Malla"));
    Malla->SetupAttachment(RootComponent);
    Malla->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    ProyectilMovimientoComponente = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProyectilMovimientoComponente"));
	ProyectilMovimientoComponente->bRotationFollowsVelocity = true;
    ProyectilMovimientoComponente->SetIsReplicated(true);
    ProyectilMovimientoComponente->bShouldBounce = true;
}

void AProyectilGranada::BeginPlay()  {
    AActor::BeginPlay();

    HumoTrazaCrear();
    IniciarTemporizadorDeFin(); 

    ProyectilMovimientoComponente->OnProjectileBounce.AddDynamic(this, &AProyectilGranada::AlRebotar);
}

void AProyectilGranada::AlRebotar(const FHitResult& ImpactoResultado, const FVector& ImpactoVelocidad) {
    if(ReboteSonido) {
        UGameplayStatics::PlaySoundAtLocation(this, ReboteSonido, GetActorLocation());
    }
}

void AProyectilGranada::Destroyed() {
    Super::Destroyed();
}



