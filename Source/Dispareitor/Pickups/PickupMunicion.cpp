#include "PickupMunicion.h"
#include "Dispareitor/Personaje/DispareitorPersonaje.h"
#include "Dispareitor/DispareitorComponentes/CombateComponente.h"


void APickupMunicion::Callback_ComponenteEsferaSolapadaInicio(UPrimitiveComponent* ComponenteSolapado, AActor* OtroActor, UPrimitiveComponent* OtroComponente, int32 OtroIndice, bool bFromSweep, const FHitResult& SweepResult) {
    Super::Callback_ComponenteEsferaSolapadaInicio(ComponenteSolapado, OtroActor, OtroComponente, OtroIndice, bFromSweep,SweepResult);      

    ADispareitorPersonaje* DispareitorPersonaje = Cast<ADispareitorPersonaje>(OtroActor);
    if(DispareitorPersonaje) {
        UCombateComponente* CombateComponente =  DispareitorPersonaje->ObtenerCombateComponente();
        if(CombateComponente) {
            CombateComponente->CogerMunicion(TipoArma, IncrementoMunicion);
        }
    }
    Destroy();
}
