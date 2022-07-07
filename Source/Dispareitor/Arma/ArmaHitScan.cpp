#include "ArmaHitScan.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Dispareitor/Personaje/DispareitorPersonaje.h"
#include "Kismet/GameplayStatics.h"

void AArmaHitScan::Disparar(const FVector& Objetivo) {
    Super::Disparar(Objetivo);

    APawn* PeonPropietario = Cast<APawn>(GetOwner());
    if(PeonPropietario == nullptr) {
        return;
    }
    AController* InstigadorControlador = PeonPropietario->GetController();

    const USkeletalMeshSocket* PuntaArmaSocket = ObtenerMalla()->GetSocketByName("MuzzleFlash"); 
    if(PuntaArmaSocket && InstigadorControlador) {
        FTransform PuntaArmaSocketTransform = PuntaArmaSocket->GetSocketTransform(ObtenerMalla());
        FVector Inicio = PuntaArmaSocketTransform.GetLocation();
        FVector Fin = Inicio + (Objetivo - Inicio) * 1.25f; // Lo incrementamos un poco para pasar el objetivo y garantizar un hit
    
        FHitResult ImpactoResultado;
        UWorld* Mundo = GetWorld();
        if(Mundo) {
            Mundo->LineTraceSingleByChannel(ImpactoResultado, Inicio, Fin, ECollisionChannel::ECC_Visibility);
            if(ImpactoResultado.bBlockingHit) {
                ADispareitorPersonaje* DispareitorPersonajeImpactado = Cast<ADispareitorPersonaje>(ImpactoResultado.GetActor());
                if(DispareitorPersonajeImpactado && HasAuthority()) {
                    UGameplayStatics::ApplyDamage(DispareitorPersonajeImpactado, Danio, InstigadorControlador, this, UDamageType::StaticClass());
                }
                if(ImpactoParticulas) {
                    UGameplayStatics::SpawnEmitterAtLocation(Mundo, ImpactoParticulas, ImpactoResultado.ImpactPoint, ImpactoResultado.ImpactNormal.Rotation());
                }
            }
        }
    }
}


