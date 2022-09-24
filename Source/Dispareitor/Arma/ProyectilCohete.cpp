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

#if WITH_EDITOR
void AProyectilCohete::PostEditChangeProperty(struct FPropertyChangedEvent& Evento) {
    Super::PostEditChangeProperty(Evento);

    FName NombrePropiedad = Evento.Property != nullptr ? Evento.Property->GetFName() : NAME_None;
    if(NombrePropiedad == GET_MEMBER_NAME_CHECKED(AProyectilCohete, VelocidadInicial) ) {
        if(CoheteMovimientoComponente) {
            CoheteMovimientoComponente->InitialSpeed = VelocidadInicial;
            CoheteMovimientoComponente->MaxSpeed = VelocidadInicial;
        }    
    }
}
#endif

void AProyectilCohete::BeginPlay() {
    Super::BeginPlay();

    // Para el proyectil cohete, tambien los clientes se encarga de manejar la colision
    if(!HasAuthority()) {
		CajaColision->OnComponentHit.AddDynamic(this, &AProyectilCohete::Callback_AlImpactar);
	}

    CrearTrazaDeHumo();

    if(SonidoMientraVuela && AtenuacionSonidoMientraVuela) {
        ComponenteSonidoMientraVuela = UGameplayStatics::SpawnSoundAttached(SonidoMientraVuela, GetRootComponent(), FName(), GetActorLocation(), EAttachLocation::KeepWorldPosition, false, 1.f, 1.f, 0.f, 
                                                                            AtenuacionSonidoMientraVuela, (USoundConcurrency *)nullptr, false);
    }

    // Si no ha chocado con nada en 10 sg provocamos su autodestruccion. Este metodo llamará de forma indirecta a Destroyed
    SetLifeSpan(10.f); 
}

void AProyectilCohete::Callback_AlImpactar(UPrimitiveComponent* ComponenteImpactante, AActor* ActorImpactado, UPrimitiveComponent* ComponenteImpactado, FVector ImpulsoNormal, const FHitResult& ImpactoResultado) {
    if(ActorImpactado == GetOwner()) { // Se ha creado UCoheteMovimientoComponente para gestionar esta casuistica
        return;
    }

    bHaImpactado = true;
   
    AplicarDanioDeExplosion();
    FinalizarFXDeVuelo();
    IniciarFXDeExplosion();
}

void AProyectilCohete::Destroyed() {
    AActor::Destroyed();

    if(!bHaImpactado) {
        FinalizarFXDeVuelo();
        IniciarFXDeExplosion();
    }
}

void AProyectilCohete::FinalizarFXDeVuelo() {
    if(Malla) {
        Malla->SetVisibility(false);
    }
    if(CajaColision) {
        CajaColision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    }
    if(ComponenteNiagaraTrazaDeHumo && ComponenteNiagaraTrazaDeHumo->GetSystemInstanceController()) {
        ComponenteNiagaraTrazaDeHumo->GetSystemInstanceController()->Deactivate();
    }
    if(ComponenteSonidoMientraVuela && ComponenteSonidoMientraVuela->IsPlaying()) {
        ComponenteSonidoMientraVuela->Stop();
    }
}

void AProyectilCohete::IniciarFXDeExplosion() {
    if(SistemaParticulasAlImpactar) {
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SistemaParticulasAlImpactar, GetActorTransform());
	}
	if(SonidoAlImpactar) {
		UGameplayStatics::PlaySoundAtLocation(this, SonidoAlImpactar, GetActorLocation());
	}
}


