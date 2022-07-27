#include "ProyectilGranada.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "GameFramework/ProjectileMovementComponent.h"

AProyectilGranada::AProyectilGranada() {
    Malla = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Malla"));
    Malla->SetupAttachment(RootComponent);
    Malla->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    ComponenteMovimientoProyectil = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ComponenteMovimientoProyectil"));
	ComponenteMovimientoProyectil->bRotationFollowsVelocity = true;
    ComponenteMovimientoProyectil->SetIsReplicated(true);
    ComponenteMovimientoProyectil->bShouldBounce = true;
}

void AProyectilGranada::BeginPlay()  {
    AActor::BeginPlay();

    CrearTrazaDeHumo();
    IniciarTemporizadorExplosion(); 

    ComponenteMovimientoProyectil->OnProjectileBounce.AddDynamic(this, &AProyectilGranada::Callback_AlRebotar);
}

void AProyectilGranada::Callback_AlRebotar(const FHitResult& ResultadoDelImpacto, const FVector& VelocidadAlImpactar) {
    if(SonidoAlRebotar) {
        UGameplayStatics::PlaySoundAtLocation(this, SonidoAlRebotar, GetActorLocation());
    }
}

void AProyectilGranada::Destroyed() {
    Super::Destroyed();
}



