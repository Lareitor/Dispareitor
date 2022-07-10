#include "ArmaHitScan.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Dispareitor/Personaje/DispareitorPersonaje.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Sound/SoundCue.h"
#include "DrawDebugHelpers.h"
#include "Kismet/KismetMathLibrary.h"
#include "Dispareitor/Tipos/TiposArma.h"


void AArmaHitScan::Disparar(const FVector& Objetivo) {
    Super::Disparar(Objetivo);

    APawn* PeonPropietario = Cast<APawn>(GetOwner());
    if(PeonPropietario == nullptr) {
        return;
    }
    AController* InstigadorControlador = PeonPropietario->GetController();

    const USkeletalMeshSocket* PuntaArmaSocket = ObtenerMalla()->GetSocketByName("MuzzleFlash"); 
    if(PuntaArmaSocket) {
        FTransform PuntaArmaSocketTransform = PuntaArmaSocket->GetSocketTransform(ObtenerMalla());
        FVector Inicio = PuntaArmaSocketTransform.GetLocation();
        FVector Fin = Inicio + (Objetivo - Inicio) * 1.25f; // Lo incrementamos un poco para pasar el objetivo y garantizar un hit
    
        FHitResult ImpactoResultado;
        UWorld* Mundo = GetWorld();
        if(Mundo) {
            Mundo->LineTraceSingleByChannel(ImpactoResultado, Inicio, Fin, ECollisionChannel::ECC_Visibility);
            FVector HumoTrazaFinal = Fin;
            if(ImpactoResultado.bBlockingHit) {
                HumoTrazaFinal = ImpactoResultado.ImpactPoint;
                ADispareitorPersonaje* DispareitorPersonajeImpactado = Cast<ADispareitorPersonaje>(ImpactoResultado.GetActor());
                if(DispareitorPersonajeImpactado && HasAuthority() && InstigadorControlador) {
                    UGameplayStatics::ApplyDamage(DispareitorPersonajeImpactado, Danio, InstigadorControlador, this, UDamageType::StaticClass());
                }
                if(ImpactoParticulas) {
                    UGameplayStatics::SpawnEmitterAtLocation(Mundo, ImpactoParticulas, ImpactoResultado.ImpactPoint, ImpactoResultado.ImpactNormal.Rotation());
                }
                if(ImpactoSonido) {
                    UGameplayStatics::PlaySoundAtLocation(this, ImpactoSonido, ImpactoResultado.ImpactPoint);
                }
            }
            if(HumoTrazaPS) {
                UParticleSystemComponent* HumoTrazaPSC = UGameplayStatics::SpawnEmitterAtLocation(Mundo, HumoTrazaPS, PuntaArmaSocketTransform);
                if(HumoTrazaPSC) {
                    HumoTrazaPSC->SetVectorParameter(FName("Target"), HumoTrazaFinal);
                }
            }
        }
        if(PuntaArmaFlash) {
            UGameplayStatics::SpawnEmitterAtLocation(Mundo, PuntaArmaFlash, PuntaArmaSocketTransform);
        }
        if(DisparoSonido) {
            UGameplayStatics::PlaySoundAtLocation(this, DisparoSonido, GetActorLocation());
        }
    }
}

FVector AArmaHitScan::PuntoFinalConDispersionCalcular(const FVector& PuntoInicial, const FVector& Objetivo) {
    FVector AObjetivoNormalizado = (Objetivo - PuntoInicial).GetSafeNormal();    
    FVector EsferaCentro = PuntoInicial + AObjetivoNormalizado * EsferaDistancia;
    FVector VectorAleatorio = UKismetMathLibrary::RandomUnitVector() * FMath::FRandRange(0.f, EsferaRadio);
    FVector LocalizacionFinal = EsferaCentro + VectorAleatorio;
    FVector ALocalizacionFinal = LocalizacionFinal - PuntoInicial;

    DrawDebugSphere(GetWorld(), EsferaCentro, EsferaRadio, 12, FColor::Red, true);
    DrawDebugSphere(GetWorld(), LocalizacionFinal, 4.f, 12, FColor::Orange, true);
    DrawDebugLine(GetWorld(), PuntoInicial, PuntoInicial + ALocalizacionFinal * RAYO_LONGITUD / ALocalizacionFinal.Size(), FColor::Cyan, true);



    return FVector(PuntoInicial + ALocalizacionFinal * RAYO_LONGITUD / ALocalizacionFinal.Size()); // Lo dividimos para no producir un overflow
}


