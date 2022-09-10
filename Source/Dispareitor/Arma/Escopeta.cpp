#include "Escopeta.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Dispareitor/Personaje/DispareitorPersonaje.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Sound/SoundCue.h"
#include "Kismet/KismetMathLibrary.h"

void AEscopeta::Disparar(const FVector& Objetivo) {
    AArma::Disparar(Objetivo);
    APawn* PeonPropietario = Cast<APawn>(GetOwner());
    if(PeonPropietario == nullptr) {
        return;
    }
    AController* InstigadorControlador = PeonPropietario->GetController();

    const USkeletalMeshSocket* PuntaArmaSocket = ObtenerMalla()->GetSocketByName("MuzzleFlash"); 
    if(PuntaArmaSocket) {
        FTransform PuntaArmaSocketTransform = PuntaArmaSocket->GetSocketTransform(ObtenerMalla());
        FVector Inicio = PuntaArmaSocketTransform.GetLocation();
        TMap<ADispareitorPersonaje*, uint32> DispareitorPersonajesImpactadosMapa;

        for(uint32 i = 0; i < NumeroPerdigones; i++) {
            FHitResult ImpactoResultado = CalcularImpacto(Inicio, Objetivo);

             ADispareitorPersonaje* DispareitorPersonajeImpactado = Cast<ADispareitorPersonaje>(ImpactoResultado.GetActor());
            if(DispareitorPersonajeImpactado && HasAuthority() && InstigadorControlador) {
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

        for(auto ElementoMapa : DispareitorPersonajesImpactadosMapa) {
            if(ElementoMapa.Key && HasAuthority() && InstigadorControlador) {
                UGameplayStatics::ApplyDamage(ElementoMapa.Key, Danio * ElementoMapa.Value, InstigadorControlador, this, UDamageType::StaticClass());
            }
        }
    }
}

void AEscopeta::CalcularPuntoFinalConDispersionParaEscopeta(const FVector& Objetivo, TArray<FVector>& Objetivos) {
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