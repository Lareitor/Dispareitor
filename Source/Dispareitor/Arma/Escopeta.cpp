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
    AController* ControladorInstigador = PeonPropietario->GetController();

    const USkeletalMeshSocket* PuntaArmaSocket = ObtenerMalla()->GetSocketByName("MuzzleFlash"); 
    if(PuntaArmaSocket) {
        const FTransform PuntaArmaSocketTransform = PuntaArmaSocket->GetSocketTransform(ObtenerMalla());
        const FVector Inicio = PuntaArmaSocketTransform.GetLocation();

        TMap<ADispareitorPersonaje*, uint32> MapaPersonajesImpactados;
        TMap<ADispareitorPersonaje*, uint32> MapaPersonajesImpactadosCabeza;
        for(FVector_NetQuantize Objetivo: Objetivos) {
            FHitResult ImpactoResultado = CalcularImpacto(Inicio, Objetivo);
           
            ADispareitorPersonaje* PersonajeImpactado = Cast<ADispareitorPersonaje>(ImpactoResultado.GetActor());
            if(PersonajeImpactado) {
                const bool bDisparoCabeza = ImpactoResultado.BoneName.ToString() == FString("head");
                if(bDisparoCabeza) {
                    if(MapaPersonajesImpactadosCabeza.Contains(PersonajeImpactado)) {
                        MapaPersonajesImpactadosCabeza[PersonajeImpactado]++;
                    } else {
                        MapaPersonajesImpactadosCabeza.Emplace(PersonajeImpactado, 1);
                    }
                } else {
                    if(MapaPersonajesImpactados.Contains(PersonajeImpactado)) {
                        MapaPersonajesImpactados[PersonajeImpactado]++;
                    } else {
                        MapaPersonajesImpactados.Emplace(PersonajeImpactado, 1);
                    }
                }                
            }
            if(SistemaParticulasAlImpactar) {
                UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SistemaParticulasAlImpactar, ImpactoResultado.ImpactPoint, ImpactoResultado.ImpactNormal.Rotation());
            }
            if(SonidoAlImpactar) {
                UGameplayStatics::PlaySoundAtLocation(this, SonidoAlImpactar, ImpactoResultado.ImpactPoint, .5f, FMath::FRandRange(-.5f, .5f));
            }
        }

        TArray<ADispareitorPersonaje*> ArrayPersonajesImpactados;
        TMap<ADispareitorPersonaje*, float> MapaDanio;
        for(auto ElementoMapa : MapaPersonajesImpactados) {
            if(ElementoMapa.Key) {
                MapaDanio.Emplace(ElementoMapa.Key, ElementoMapa.Value * Danio);
                ArrayPersonajesImpactados.AddUnique(ElementoMapa.Key);
            }
        }
        for(auto ElementoMapaCabeza : MapaPersonajesImpactadosCabeza) {
            if(ElementoMapaCabeza.Key) {
                if(MapaDanio.Contains(ElementoMapaCabeza.Key)) {
                    MapaDanio[ElementoMapaCabeza.Key] += ElementoMapaCabeza.Value * DanioEnCabeza;
                } else {
                    MapaDanio.Emplace(ElementoMapaCabeza.Key, ElementoMapaCabeza.Value * DanioEnCabeza); 
                }
                ArrayPersonajesImpactados.AddUnique(ElementoMapaCabeza.Key);
            }
        }

        for(auto ElementoDanio : MapaDanio) {
            if(ElementoDanio.Key && ControladorInstigador) {
                if(HasAuthority() && (!bRebobinarLadoServidor || PeonPropietario->IsLocallyControlled())) {
                    UGameplayStatics::ApplyDamage(ElementoDanio.Key, ElementoDanio.Value, ControladorInstigador, this, UDamageType::StaticClass());
                }
            }
        }       

        if(!HasAuthority() && bRebobinarLadoServidor) {
            DispareitorPersonaje = DispareitorPersonaje ? DispareitorPersonaje : Cast<ADispareitorPersonaje>(PeonPropietario);    
            DispareitorControladorJugador = DispareitorControladorJugador ? DispareitorControladorJugador : Cast<ADispareitorControladorJugador>(ControladorInstigador);
            if(DispareitorPersonaje && DispareitorPersonaje->ObtenerCompensacionLagComponente() && DispareitorPersonaje->IsLocallyControlled() && DispareitorControladorJugador && DispareitorPersonaje->IsLocallyControlled()) {
                DispareitorPersonaje->ObtenerCompensacionLagComponente()->PeticionImpactoEscopeta_EnServidor(ArrayPersonajesImpactados, Inicio, Objetivos, DispareitorControladorJugador->ObtenerTiempoServidor() - DispareitorControladorJugador->STT);
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