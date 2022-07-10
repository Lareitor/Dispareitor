#include "Escopeta.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Dispareitor/Personaje/DispareitorPersonaje.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Sound/SoundCue.h"

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
        for(uint32 i = 0; i < PerdigonesNumero; i++) {
            FVector Fin = PuntoFinalConDispersionCalcular(Inicio, Objetivo);
        }
    }
}