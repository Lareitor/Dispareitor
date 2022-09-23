#include "ArmaHitScan.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Dispareitor/Personaje/DispareitorPersonaje.h"
#include "Dispareitor/ControladorJugador/DispareitorControladorJugador.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Sound/SoundCue.h"
#include "DrawDebugHelpers.h"
#include "Dispareitor/Tipos/TiposArma.h"
#include "Dispareitor/DispareitorComponentes/CompensacionLagComponente.h"


void AArmaHitScan::Disparar(const FVector& Objetivo) {
    Super::Disparar(Objetivo);

    APawn* PeonPropietario = Cast<APawn>(GetOwner());
    if(PeonPropietario == nullptr) {
        return;
    }
    AController* ControladorDelInstigador = PeonPropietario->GetController();

    const USkeletalMeshSocket* SocketPuntaArma = ObtenerMalla()->GetSocketByName("MuzzleFlash"); 
    if(SocketPuntaArma) {
        FTransform TransformSocketPuntaArma = SocketPuntaArma->GetSocketTransform(ObtenerMalla());
        FVector Inicio = TransformSocketPuntaArma.GetLocation();
        FHitResult ImpactoResultado = CalcularImpacto(Inicio, Objetivo);

        ADispareitorPersonaje* DispareitorPersonajeImpactado = Cast<ADispareitorPersonaje>(ImpactoResultado.GetActor());
        if(DispareitorPersonajeImpactado && ControladorDelInstigador) {
            if(HasAuthority() && !bRebobinarLadoServidor) {
                UGameplayStatics::ApplyDamage(DispareitorPersonajeImpactado, Danio, ControladorDelInstigador, this, UDamageType::StaticClass());
            } 
            if (!HasAuthority() && bRebobinarLadoServidor) {
                DispareitorPersonaje = DispareitorPersonaje != nullptr ? DispareitorPersonaje : Cast<ADispareitorPersonaje>(PeonPropietario);    
                DispareitorControladorJugador = DispareitorControladorJugador != nullptr ? DispareitorControladorJugador : Cast<ADispareitorControladorJugador>(ControladorDelInstigador);
                if(DispareitorPersonaje && DispareitorPersonaje->ObtenerCompensacionLagComponente() && DispareitorPersonaje->IsLocallyControlled() && DispareitorControladorJugador) {
                    DispareitorPersonaje->ObtenerCompensacionLagComponente()->PeticionImpacto_EnServidor(DispareitorPersonajeImpactado, Inicio, Objetivo, DispareitorControladorJugador->ObtenerTiempoServidor() - DispareitorControladorJugador->STT, this);
                }
            }
        }
        if(SistemaParticulasAlImpactar) {
            UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SistemaParticulasAlImpactar, ImpactoResultado.ImpactPoint, ImpactoResultado.ImpactNormal.Rotation());
        }
        if(SonidoAlImpactar) {
            UGameplayStatics::PlaySoundAtLocation(this, SonidoAlImpactar, ImpactoResultado.ImpactPoint);
        }
        if(SistemaParticulasFogonazoEnPuntaArma) {
            UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SistemaParticulasFogonazoEnPuntaArma, TransformSocketPuntaArma);
        }
        if(SonidoAlDisparar) {
            UGameplayStatics::PlaySoundAtLocation(this, SonidoAlDisparar, GetActorLocation());
        }
    }
}

FHitResult AArmaHitScan::CalcularImpacto(const FVector& Inicio, const FVector& Objetivo) {
    FHitResult ImpactoResultado;
    UWorld* Mundo = GetWorld();
    if(Mundo) {
        // Lo incrementamos un poco para pasar el objetivo y garantizar un hit
        FVector Fin = Inicio + (Objetivo - Inicio) * 1.25f;
        Mundo->LineTraceSingleByChannel(ImpactoResultado, Inicio, Fin, ECollisionChannel::ECC_Visibility);
        FVector HumoTrazaFinal = Fin; 
        if(ImpactoResultado.bBlockingHit) {
             HumoTrazaFinal = ImpactoResultado.ImpactPoint;
        }

        //DrawDebugSphere(GetWorld(), HumoTrazaFinal, 16.f, 12, FColor::Orange, true);

        if(SistemaParticulasTrazaDeHumo) {
            UParticleSystemComponent* ComponenteSistemaParticulasTrazaDeHumo = UGameplayStatics::SpawnEmitterAtLocation(Mundo, SistemaParticulasTrazaDeHumo, Inicio, FRotator::ZeroRotator, true);
            if(ComponenteSistemaParticulasTrazaDeHumo) {
                ComponenteSistemaParticulasTrazaDeHumo->SetVectorParameter(FName("Target"), HumoTrazaFinal);
            }
        }
    }

    return ImpactoResultado;
}


