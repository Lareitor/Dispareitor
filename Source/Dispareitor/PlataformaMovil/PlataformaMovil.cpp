#include "PlataformaMovil.h"
#include "Components/InterpToMovementComponent.h"

APlataformaMovil::APlataformaMovil() {
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	SetReplicatingMovement(true);

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("ComponenteRaiz"));

	ComponenteMallaEstatica = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ComponenteMallaEstatica"));
	ComponenteMallaEstatica->SetupAttachment(RootComponent);

	ComponenteInterpolacionMovimiento = CreateDefaultSubobject<UInterpToMovementComponent>(TEXT("ComponenteInterpolacionMovimiento"));
}

void APlataformaMovil::BeginPlay() {
	Super::BeginPlay();	

	if(HasAuthority()) {
		ComponenteInterpolacionMovimiento->BehaviourType = EInterpToBehaviourType::PingPong; // Debe estar en el mismo metodo donde se cambian los puntos
		TArray<FInterpControlPoint> PuntosDeControl;
		PuntosDeControl.Add(FInterpControlPoint(PosicionInicial, true));
		PuntosDeControl.Add(FInterpControlPoint(PosicionFinal, true));
		ComponenteInterpolacionMovimiento->ControlPoints = PuntosDeControl;
		ComponenteInterpolacionMovimiento->FinaliseControlPoints(); // Importante para que actualice los puntos y se mueva la plataforma
		ComponenteInterpolacionMovimiento->Duration = Duracion;
	}
    
}

void APlataformaMovil::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}

