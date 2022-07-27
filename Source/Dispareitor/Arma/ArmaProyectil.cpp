#include "ArmaProyectil.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Proyectil.h"

// Hereda de AArma

void AArmaProyectil::Disparar(const FVector& Objetivo) {
    Super::Disparar(Objetivo);

    if(!HasAuthority()) {
        return;
    }

    APawn* PeonInstigador = Cast<APawn>(GetOwner());
    const USkeletalMeshSocket* SocketPuntaArma = ObtenerMalla()->GetSocketByName(FName("MuzzleFlash"));
    if(SocketPuntaArma) {
        FTransform TransformSocketPuntaArma = SocketPuntaArma->GetSocketTransform(ObtenerMalla());
        FVector VectorAObjetivo = Objetivo - TransformSocketPuntaArma.GetLocation();
        FRotator RotacionAObjetivo = VectorAObjetivo.Rotation();
        if(ClaseProyectil && PeonInstigador) {
            FActorSpawnParameters SpawnParametros;
            SpawnParametros.Owner = GetOwner();
            SpawnParametros.Instigator = PeonInstigador;
            UWorld* Mundo = GetWorld();
            if(Mundo) {
                Mundo->SpawnActor<AProyectil>(ClaseProyectil, TransformSocketPuntaArma.GetLocation(), RotacionAObjetivo, SpawnParametros);
            }
        }
    }
}


