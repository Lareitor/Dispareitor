#include "PlataformaMovil.h"
#include "Components/InterpToMovementComponent.h"

APlataformaMovil::APlataformaMovil(){
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("ComponenteRaiz"));

	ComponenteMallaEstatica = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ComponenteMallaEstatica"));
	ComponenteMallaEstatica->SetupAttachment(RootComponent);

	ComponenteInterpolacionMovimiento = CreateDefaultSubobject<UInterpToMovementComponent>(TEXT("ComponenteInterpolacionMovimiento"));
	ComponenteInterpolacionMovimiento->BehaviourType = EInterpToBehaviourType::PingPong;
	TArray<FInterpControlPoint> PuntosDeControl;
	PosicionInicial = FVector(0.f, 0.f, 0.f);
	PosicionFinal = FVector(0.f, 500.f, 0.f);
	PuntosDeControl.Add(FInterpControlPoint(PosicionInicial, true));
	PuntosDeControl.Add(FInterpControlPoint(PosicionFinal, true));
	ComponenteInterpolacionMovimiento->ControlPoints = PuntosDeControl;
	ComponenteInterpolacionMovimiento->Duration = 5.0f;
}

void APlataformaMovil::BeginPlay() {
	Super::BeginPlay();
}

void APlataformaMovil::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}

