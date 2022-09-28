#include "Escopeta.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Dispareitor/Personaje/DispareitorPersonaje.h"
#include "Dispareitor/ControladorJugador/DispareitorControladorJugador.h"
#include "Dispareitor/DispareitorComponentes/CompensacionLagComponente.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Sound/SoundCue.h"
#include "Kismet/KismetMathLibrary.h"

void AEscopeta::DispararEscopeta(const TArray<FVector_NetQuantize>& Objetivos) {
    AArma::Disparar(FVector());
    APawn* PeonPropietario = Cast<APawn>(GetOwner());
    if(!PeonPropietario) {
        return;
    }
    AController* InstigadorControlador = PeonPropietario->GetController();

    const USkeletalMeshSocket* PuntaArmaSocket = ObtenerMalla()->GetSocketByName("MuzzleFlash"); 
    if(PuntaArmaSocket) {
        const FTransform PuntaArmaSocketTransform = PuntaArmaSocket->GetSocketTransform(ObtenerMalla());
        const FVector Inicio = PuntaArmaSocketTransform.GetLocation();

        TMap<ADispareitorPersonaje*, uint32> DispareitorPersonajesImpactadosMapa;
        for(FVector_NetQuantize Objetivo: Objetivos) {
            FHitResult ImpactoResultado = CalcularImpacto(Inicio, Objetivo);
           
            ADispareitorPersonaje* DispareitorPersonajeImpactado = Cast<ADispareitorPersonaje>(ImpactoResultado.GetActor());
            if(DispareitorPersonajeImpactado) {
                if(DispareitorPersonajesImpactadosMapa.Contains(DispareitorPersonajeImpactado)) {
                    DispareitorPersonajesImpactadosMapa[DispareitorPersonajeImpactado]++;
                } else {
                    DispareitorPersonajesImpactadosMapa.Emplace(DispareitorPersonajeImpactado, 1);
                }
            }
            if(SistemaParticulasAlImpactar) {
                UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SistemaParticulasAlImpactar, ImpactoResultado.ImpactPoint, ImpactoResultado.ImpactNormal.Rotation());
            }
            if(SonidoAlImpactar) {
                UGameplayStatics::PlaySoundAtLocation(this, SonidoAlImpactar, ImpactoResultado.ImpactPoint, .5f, FMath::FRandRange(-.5f, .5f));
            }
        }

        TArray<ADispareitorPersonaje*> DispareitorPersonajesImpactados;
        for(auto ElementoMapa : DispareitorPersonajesImpactadosMapa) {
            if(ElementoMapa.Key && InstigadorControlador) {
                if(HasAuthority() && (!bRebobinarLadoServidor || PeonPropietario->IsLocallyControlled())) {
                    UGameplayStatics::ApplyDamage(ElementoMapa.Key, Danio * ElementoMapa.Value, InstigadorControlador, this, UDamageType::StaticClass());
                }
                DispareitorPersonajesImpactados.Add(ElementoMapa.Key);
            }
        }

        if(!HasAuthority() && bRebobinarLadoServidor) {
            DispareitorPersonaje = DispareitorPersonaje != nullptr ? DispareitorPersonaje : Cast<ADispareitorPersonaje>(PeonPropietario);    
            DispareitorControladorJugador = DispareitorControladorJugador != nullptr ? DispareitorControladorJugador : Cast<ADispareitorControladorJugador>(InstigadorControlador);
            if(DispareitorPersonaje && DispareitorPersonaje->ObtenerCompensacionLagComponente() && DispareitorPersonaje->IsLocallyControlled() && DispareitorControladorJugador && DispareitorPersonaje->IsLocallyControlled()) {
                DispareitorPersonaje->ObtenerCompensacionLagComponente()->PeticionImpactoEscopeta_EnServidor(DispareitorPersonajesImpactados, Inicio, Objetivos, DispareitorControladorJugador->ObtenerTiempoServidor() - DispareitorControladorJugador->STT);
            }
        }
    }
}


void AEscopeta::CalcularPuntosFinalesConDispersionParaEscopeta(const FVector& Objetivo, TArray<FVector_NetQuantize>& Objetivos) {
    const USkeletalMeshSocket* SocketPuntaArma = ObtenerMalla()->GetSocketByName("MuzzleFlash"); 
    
    if(SocketPuntaArma) {
        const FTransform TransformSocketPuntaArma = SocketPuntaArma->GetSocketTransform(ObtenerMalla());
        const FVector Inicio = TransformSocketPuntaArma.GetLocation();
        const FVector AObjetivoNormalizado = (Objetivo - Inicio).GetSafeNormal();    
        const FVector EsferaCentro = Inicio + AObjetivoNormalizado * DistanciaAEsferaDeDispersion;
           
        for(uint32 i = 0; i < NumeroPerdigones; i++) {
            const FVector VectorAleatorio = UKismetMathLibrary::RandomUnitVector() * FMath::FRandRange(0.f, RadioDeEsferaDeDispersion);
            const FVector LocalizacionFinal = EsferaCentro + VectorAleatorio;
            FVector ALocalizacionFinal = LocalizacionFinal - Inicio;
            ALocalizacionFinal = Inicio + ALocalizacionFinal * RAYO_LONGITUD / ALocalizacionFinal.Size(); // Lo dividimos para no producir un overflow
            Objetivos.Add(ALocalizacionFinal);
        }
    }
}