#include "LimitesJuego.h"
#include "Dispareitor/Personaje/DispareitorPersonaje.h"
#include "Kismet/GameplayStatics.h"

ALimitesJuego::ALimitesJuego() {
    GetStaticMeshComponent()->SetGenerateOverlapEvents(true);
    GetStaticMeshComponent()->OnComponentBeginOverlap.AddDynamic(this, &ALimitesJuego::CallbackMallaSolapadoInicio);		
}

void ALimitesJuego::CallbackMallaSolapadoInicio(UPrimitiveComponent* ComponenteSolapado, AActor* OtroActor, UPrimitiveComponent* OtroComponente, int32 OtroIndice, bool bFromSweep, const FHitResult& SweepResult) { 
	ADispareitorPersonaje* DispareitorPersonaje = Cast<ADispareitorPersonaje>(OtroActor);
	if(DispareitorPersonaje && HasAuthority()) {
		AController* ControladorPersonaje = DispareitorPersonaje->Controller; // Es el propio controlador del personaje (se hace daño asimismo)
		UGameplayStatics::ApplyDamage(DispareitorPersonaje, 1000.f, ControladorPersonaje, this, UDamageType::StaticClass());
	}
}