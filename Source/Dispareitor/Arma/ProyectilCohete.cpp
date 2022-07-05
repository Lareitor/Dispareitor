#include "ProyectilCohete.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Sound/SoundCue.h"
#include "Components/BoxComponent.h"
#include "Components/AudioComponent.h"


AProyectilCohete::AProyectilCohete() {
    Malla = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Malla"));
    Malla->SetupAttachment(RootComponent);
    Malla->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AProyectilCohete::BeginPlay() {
    Super::BeginPlay();

    // Para el proyectil cohete, tambien los clientes se encarga de manejar la colision
    if(!HasAuthority()) {
		CajaColision->OnComponentHit.AddDynamic(this, &AProyectilCohete::CallbackAlImpactar);
	}

    if(HumoTraza) {
        HumoTrazaComponente = UNiagaraFunctionLibrary::SpawnSystemAttached(HumoTraza, GetRootComponent(), FName(), GetActorLocation(), GetActorRotation(), EAttachLocation::KeepWorldPosition, false);
    }

    if(SonidoMientraVuela && SonidoMientraVuelaAtenuacion) {
        SonidoMientraVuelaComponente = UGameplayStatics::SpawnSoundAttached(SonidoMientraVuela, GetRootComponent(), FName(), GetActorLocation(), EAttachLocation::KeepWorldPosition, false, 1.f, 1.f, 0.f, 
                                                                            SonidoMientraVuelaAtenuacion, (USoundConcurrency *)nullptr, false);
    }
}

void AProyectilCohete::CallbackAlImpactar(UPrimitiveComponent* ComponenteImpactante, AActor* ActorImpactado, UPrimitiveComponent* ComponenteImpactado, FVector ImpulsoNormal, const FHitResult& ImpactoResultado) {
    APawn* PeonQueDispara = GetInstigator();  
    if(PeonQueDispara && HasAuthority()) {
        AController* ControladorDeQuienDispara = PeonQueDispara->GetController();
        if(ControladorDeQuienDispara) {
            UGameplayStatics::ApplyRadialDamageWithFalloff(this, Dano, 10.f, GetActorLocation(), 200.f, 500.f, 1.f, UDamageType::StaticClass(), TArray<AActor*>(), this, ControladorDeQuienDispara);
        }
    }
    
    GetWorldTimerManager().SetTimer(DestruirTemporizador, this, &AProyectilCohete::DestruirTemporizadorFinalizado, DestruirTiempo);

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

void AProyectilCohete::DestruirTemporizadorFinalizado() {
    Destroy();
}

// Para evitar que se llame a Proyectil::Destroyed sobreescribimos este metodo a vacio
void AProyectilCohete::Destroyed() {
}
