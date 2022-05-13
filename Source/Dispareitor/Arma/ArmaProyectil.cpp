#include "ArmaProyectil.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Proyectil.h"

void AArmaProyectil::Disparar(const FVector& Objetivo) {
    Super::Disparar(Objetivo);
    APawn* PeonInstigador = Cast<APawn>(GetOwner());

    const USkeletalMeshSocket* MuzzleFlashSocket = ObtenerMalla()->GetSocketByName(FName("MuzzleFlash"));
    if(MuzzleFlashSocket) {
        FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform(ObtenerMalla());
        FVector VectorAObjetivo = Objetivo - SocketTransform.GetLocation();
        FRotator RotacionAObjetivo = VectorAObjetivo.Rotation();
        if(Proyectil && PeonInstigador) {
            FActorSpawnParameters SpawnParametros;
            SpawnParametros.Owner = GetOwner();
            SpawnParametros.Instigator = PeonInstigador;
            UWorld* Mundo = GetWorld();
            if(Mundo) {
                Mundo->SpawnActor<AProyectil>(Proyectil, SocketTransform.GetLocation(), RotacionAObjetivo, SpawnParametros);
            }
        }
    }
}


