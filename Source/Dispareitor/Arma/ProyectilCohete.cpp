#include "ProyectilCohete.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraComponent.h"
#include "Sound/SoundCue.h"
#include "Components/BoxComponent.h"
#include "Components/AudioComponent.h"
#include "CoheteMovimientoComponente.h"


AProyectilCohete::AProyectilCohete() {
    Malla = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Malla"));
    Malla->SetupAttachment(RootComponent);
    Malla->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    CoheteMovimientoComponente = CreateDefaultSubobject<UCoheteMovimientoComponente>(TEXT("CoheteMovimientoComponente"));
    CoheteMovimientoComponente->bRotationFollowsVelocity = true;
    CoheteMovimientoComponente->SetIsReplicated(true);
}

void AProyectilCohete::BeginPlay() {
    Super::BeginPlay();

    // Para el proyectil cohete, tambien los clientes se encarga de manejar la colision
    if(!HasAuthority()) {
		CajaColision->OnComponentHit.AddDynamic(this, &AProyectilCohete::CallbackAlImpactar);
	}

    HumoTrazaCrear();

    if(SonidoMientraVuela && SonidoMientraVuelaAtenuacion) {
        SonidoMientraVuelaComponente = UGameplayStatics::SpawnSoundAttached(SonidoMientraVuela, GetRootComponent(), FName(), GetActorLocation(), EAttachLocation::KeepWorldPosition, false, 1.f, 1.f, 0.f, 
                                                                            SonidoMientraVuelaAtenuacion, (USoundConcurrency *)nullptr, false);
    }
}

void AProyectilCohete::CallbackAlImpactar(UPrimitiveComponent* ComponenteImpactante, AActor* ActorImpactado, UPrimitiveComponent* ComponenteImpactado, FVector ImpulsoNormal, const FHitResult& ImpactoResultado) {
    if(ActorImpactado == GetOwner()) { // Se ha creado UCoheteMovimientoComponente para gestionar esta casuistica
        return;
    }
   
    ExplosionDanioAplicar();
    
    IniciarTemporizadorDeFin();

    if(ImpactoParticulas) {
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactoParticulas, GetActorTransform());
	}
	if(ImpactoSonido) {
		UGameplayStatics::PlaySoundAtLocation(this, ImpactoSonido, GetActorLocation());
	}
    if(Malla) {
        Malla->SetVisibility(false);
    }
    if(CajaColision) {
        CajaColision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    }
    if(HumoTrazaComponente && HumoTrazaComponente->GetSystemInstance()) {
        HumoTrazaComponente->GetSystemInstance()->Deactivate();
    }
    if(SonidoMientraVuelaComponente && SonidoMientraVuelaComponente->IsPlaying()) {
        SonidoMientraVuelaComponente->Stop();
    }
}

// Para evitar que se llame a Proyectil::Destroyed sobreescribimos este metodo a vacio
void AProyectilCohete::Destroyed() {
}
