#include "ArmaHitScan.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Dispareitor/Personaje/DispareitorPersonaje.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Particles/ParticleSystemComponent.h"

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
            }
            if(HumoTrazaPS) {
                UParticleSystemComponent* HumoTrazaPSC = UGameplayStatics::SpawnEmitterAtLocation(Mundo, HumoTrazaPS, PuntaArmaSocketTransform);
                if(HumoTrazaPSC) {
                    HumoTrazaPSC->SetVectorParameter(FName("Target"), HumoTrazaFinal);
                }
            }
        }
    }
}


