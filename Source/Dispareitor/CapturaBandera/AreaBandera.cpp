#include "AreaBandera.h"
#include "Components/SphereComponent.h"
#include "Dispareitor/Arma/ArmaBandera.h"
#include "Dispareitor/ModoJuego/DispareitorModoJuegoEquiposBande.h"

AAreaBandera::AAreaBandera() {
	PrimaryActorTick.bCanEverTick = false;
	Area = CreateDefaultSubobject<USphereComponent>(TEXT("Area"));
	SetRootComponent(Area);
}

void AAreaBandera::BeginPlay() {
	Super::BeginPlay();	

	Area->OnComponentBeginOverlap.AddDynamic(this, &AAreaBandera::Callback_EsferaSolapadaInicio);
}

void AAreaBandera::Callback_EsferaSolapadaInicio(UPrimitiveComponent* ComponenteSolapado, AActor* OtroActor, UPrimitiveComponent* OtroComponente, int32 OtroIndice, bool bFromSweep, const FHitResult& SweepResult) {
	AArmaBandera* ArmaBanderaSolapada = Cast<AArmaBandera>(OtroActor);
	if(ArmaBanderaSolapada && ArmaBanderaSolapada->ObtenerEquipo() != Equipo) {
		ADispareitorModoJuegoEquiposBande* DModoJuegoEquiposBande = GetWorld()->GetAuthGameMode<ADispareitorModoJuegoEquiposBande>();
		if(DModoJuegoEquiposBande) {			
			DModoJuegoEquiposBande->BanderaCapturada(ArmaBanderaSolapada, this);			
		}
		ArmaBanderaSolapada->Resetear();
	}
}

