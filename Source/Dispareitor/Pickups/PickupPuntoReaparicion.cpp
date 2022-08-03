#include "PickupPuntoReaparicion.h"
#include "Pickup.h"

APickupPuntoReaparicion::APickupPuntoReaparicion() {
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
}

void APickupPuntoReaparicion::BeginPlay() {
	Super::BeginPlay();
	IniciarTemporizadorReaparicionPickup((AActor*)nullptr);
}

void APickupPuntoReaparicion::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}

void APickupPuntoReaparicion::IniciarTemporizadorReaparicionPickup(AActor *ActorDestruido) {
	const float TiempoReaparicion = FMath::FRandRange(TiempoReaparicionPickupMinimo, TiempoReaparicionPickupMaximo);
	GetWorldTimerManager().SetTimer(TemporizadorReaparicionPickup, this, &APickupPuntoReaparicion::Callback_TemporizadorReaparicionPickupFinalizado, TiempoReaparicion);
}

void APickupPuntoReaparicion::Callback_TemporizadorReaparicionPickupFinalizado() {
	if(HasAuthority()) {
		ReaparecerPickup();
	}
}

void APickupPuntoReaparicion::ReaparecerPickup() {
	int32 NumeroClasesPickup = ClasesPickup.Num();
	if(NumeroClasesPickup > 0) {
		int32 Seleccion = FMath::RandRange(0, NumeroClasesPickup - 1);
		PickupReaparecido = GetWorld()->SpawnActor<APickup>(ClasesPickup[Seleccion], GetActorTransform());
		if(PickupReaparecido && HasAuthority()) {
			PickupReaparecido->OnDestroyed.AddDynamic(this, &APickupPuntoReaparicion::IniciarTemporizadorReaparicionPickup);
		}
	}
}




