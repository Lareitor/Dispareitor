#include "PickupVida.h"
#include "Dispareitor/Personaje/DispareitorPersonaje.h"
#include "Dispareitor/DispareitorComponentes/BuffComponente.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"

APickupVida::APickupVida() {
    bReplicates = true;
    ComponenteNiagaraVida = CreateDefaultSubobject<UNiagaraComponent>(TEXT("ComponenteNiagaraVida"));
    ComponenteNiagaraVida->SetupAttachment(RootComponent);

}

void APickupVida::Callback_ComponenteEsferaSolapadaInicio(UPrimitiveComponent* ComponenteSolapado, AActor* OtroActor, UPrimitiveComponent* OtroComponente, int32 OtroIndice, bool bFromSweep, const FHitResult& SweepResult) {
    Super::Callback_ComponenteEsferaSolapadaInicio(ComponenteSolapado, OtroActor, OtroComponente, OtroIndice, bFromSweep,SweepResult);      

    ADispareitorPersonaje* DispareitorPersonaje = Cast<ADispareitorPersonaje>(OtroActor);
    if(DispareitorPersonaje) {
        UBuffComponente* BuffComponente = DispareitorPersonaje->ObtenerBuffComponente();
        if(BuffComponente) {
            BuffComponente->Sanar(IncrementoVida, TiempoIncrementoVida);
        }
    }
    Destroy();
}

void APickupVida::Destroyed() {
    if(SistemaNiagaraVida) {
        UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, SistemaNiagaraVida, GetActorLocation(), GetActorRotation());
    }
    Super::Destroyed();
}