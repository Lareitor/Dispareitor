#include "ArmaProyectil.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Proyectil.h"

// Hereda de AArma

void AArmaProyectil::Disparar(const FVector& Objetivo) {
    Super::Disparar(Objetivo);

    APawn* PeonInstigador = Cast<APawn>(GetOwner());
    const USkeletalMeshSocket* SocketPuntaArma = ObtenerMalla()->GetSocketByName(FName("MuzzleFlash"));
    UWorld* Mundo = GetWorld();

    if(PeonInstigador && SocketPuntaArma && Mundo) {
        FTransform TransformSocketPuntaArma = SocketPuntaArma->GetSocketTransform(ObtenerMalla());
        FVector VectorAObjetivo = Objetivo - TransformSocketPuntaArma.GetLocation();
        FRotator RotacionAObjetivo = VectorAObjetivo.Rotation();
        
        FActorSpawnParameters SpawnParametros;
        SpawnParametros.Owner = GetOwner();
        SpawnParametros.Instigator = PeonInstigador;

        AProyectil* ProyectilGenerado = nullptr;
        if(bRebobinarLadoServidor) {
            if(PeonInstigador->HasAuthority()) { // Servidor...
                if(PeonInstigador->IsLocallyControlled()) { // ... y controlado localmente -> usar proyectil replicado y no SSR (Server Side Rewind)
                    ProyectilGenerado = Mundo->SpawnActor<AProyectil>(ClaseProyectil, TransformSocketPuntaArma.GetLocation(), RotacionAObjetivo, SpawnParametros);         
                    ProyectilGenerado->bRebobinarLadoServidor = false;
                    ProyectilGenerado->Danio = Danio;
                } else { // ... y no controlado locamente -> generar un proyectil no replicado y no SSR 
                    ProyectilGenerado = Mundo->SpawnActor<AProyectil>(ClaseProyectilNoReplicado, TransformSocketPuntaArma.GetLocation(), RotacionAObjetivo, SpawnParametros);         
                    ProyectilGenerado->bRebobinarLadoServidor = false;
                }   
            } else { // Cliente, usando SSR
                if(PeonInstigador->IsLocallyControlled()) { // controlado localmente -> generar proyectil no replicado y SSR 
                    ProyectilGenerado = Mundo->SpawnActor<AProyectil>(ClaseProyectilNoReplicado, TransformSocketPuntaArma.GetLocation(), RotacionAObjetivo, SpawnParametros);         
                    ProyectilGenerado->bRebobinarLadoServidor = true;
                    ProyectilGenerado->InicioRayo = TransformSocketPuntaArma.GetLocation();
                    ProyectilGenerado->VectorVelocidadInicial = ProyectilGenerado->GetActorForwardVector() * ProyectilGenerado->VelocidadInicial; // GetActorForwardVector está normalizado por eso lo multiplicamos
                    ProyectilGenerado->Danio = Danio;
                } else { // no controlado locamente -> generar no replicado y no SSR
                    ProyectilGenerado = Mundo->SpawnActor<AProyectil>(ClaseProyectilNoReplicado, TransformSocketPuntaArma.GetLocation(), RotacionAObjetivo, SpawnParametros);         
                    ProyectilGenerado->bRebobinarLadoServidor = false;
                }
            } 
        } else { // Arma no usa SSR
            if(PeonInstigador->HasAuthority()) { 
                ProyectilGenerado = Mundo->SpawnActor<AProyectil>(ClaseProyectil, TransformSocketPuntaArma.GetLocation(), RotacionAObjetivo, SpawnParametros);         
                ProyectilGenerado->bRebobinarLadoServidor = false;
                ProyectilGenerado->Danio = Danio;
            }
        }
    }
}


