#include "PickupVelocidad.h"
#include "Dispareitor/Personaje/DispareitorPersonaje.h"
#include "Dispareitor/DispareitorComponentes/BuffComponente.h"

void APickupVelocidad::Callback_ComponenteEsferaSolapadaInicio(UPrimitiveComponent* ComponenteSolapado, AActor* OtroActor, UPrimitiveComponent* OtroComponente, int32 OtroIndice, bool bFromSweep, const FHitResult& SweepResult) {
    Super::Callback_ComponenteEsferaSolapadaInicio(ComponenteSolapado, OtroActor, OtroComponente, OtroIndice, bFromSweep,SweepResult);      

    ADispareitorPersonaje* DispareitorPersonaje = Cast<ADispareitorPersonaje>(OtroActor);
    if(DispareitorPersonaje) {
        UBuffComponente* BuffComponente = DispareitorPersonaje->ObtenerBuffComponente();
        if(BuffComponente) {
            BuffComponente->AumentarVelocidad(VelocidadDePie, VelocidadAgachado, Duracion);
        }
    }
    Destroy();
}

