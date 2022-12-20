#include "LimitesJuego.h"
#include "Dispareitor/Personaje/DispareitorPersonaje.h"
#include "Dispareitor/Arma/Arma.h"
#include "Dispareitor/ModoJuego/DispareitorModoJuego.h"
#include "Kismet/GameplayStatics.h"

ALimitesJuego::ALimitesJuego() {
    GetStaticMeshComponent()->SetGenerateOverlapEvents(true);
    GetStaticMeshComponent()->OnComponentBeginOverlap.AddDynamic(this, &ALimitesJuego::Callback_SolapadaMallaInicio);		
}

void ALimitesJuego::Callback_SolapadaMallaInicio(UPrimitiveComponent* ComponenteSolapado, AActor* OtroActor, UPrimitiveComponent* OtroComponente, int32 OtroIndice, bool bFromSweep, const FHitResult& SweepResult) { 	
	if(!HasAuthority()) {
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("ALimitesJuego.Callback_SolapadaMallaInicio: %s"), *OtroActor->GetActorNameOrLabel());
	
	ADispareitorPersonaje* DispareitorPersonaje = Cast<ADispareitorPersonaje>(OtroActor);
	if(DispareitorPersonaje) {
		AController* ControladorPersonaje = DispareitorPersonaje->Controller; // Es el propio controlador del personaje (se hace daño asimismo)
		UGameplayStatics::ApplyDamage(DispareitorPersonaje, 1000.f, ControladorPersonaje, this, UDamageType::StaticClass());
	} else {
		AArma* Arma = Cast<AArma>(OtroActor);
		ADispareitorModoJuego* DModoJuego = GetWorld()->GetAuthGameMode<ADispareitorModoJuego>();
		if(!Arma || !DModoJuego) {
			return;
		}
		Arma->DeshabilitarColisiones();
		DModoJuego->SituarArmaTrasCaerEnLimitesJuego(Arma);
		Arma->HabilitarColisiones();
	}			
}