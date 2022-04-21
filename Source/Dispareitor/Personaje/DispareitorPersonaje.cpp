#include "DispareitorPersonaje.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"


ADispareitorPersonaje::ADispareitorPersonaje() {
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BrazoCamara = CreateDefaultSubobject<USpringArmComponent>(TEXT("BrazoCamara"));
	BrazoCamara->SetupAttachment(GetMesh());
	BrazoCamara->TargetArmLength = 600.f;
	BrazoCamara->bUsePawnControlRotation = true; 

	Camara = CreateDefaultSubobject<UCameraComponent>(TEXT("Camara"));
	Camara->SetupAttachment(BrazoCamara, USpringArmComponent::SocketName);
	Camara->bUsePawnControlRotation = false;
}

void ADispareitorPersonaje::BeginPlay() {
	Super::BeginPlay();	
}

void ADispareitorPersonaje::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ADispareitorPersonaje::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) {
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("Saltar", IE_Pressed, this, &ADispareitorPersonaje::Jump);

	PlayerInputComponent->BindAxis("MoverAdelanteAtras", this, &ADispareitorPersonaje::MoverAdelanteAtras);
	PlayerInputComponent->BindAxis("MoverIzquierdaDerecha", this, &ADispareitorPersonaje::MoverIzquierdaDerecha);
	PlayerInputComponent->BindAxis("Girar", this, &ADispareitorPersonaje::Girar);
	PlayerInputComponent->BindAxis("MoverArribaAbajo", this, &ADispareitorPersonaje::MoverArribaAbajo);
}

void ADispareitorPersonaje::MoverAdelanteAtras(float Valor) {
	if(Controller != nullptr && Valor != 0.f) {
		const FRotator RotacionGiro(0.f, Controller->GetControlRotation().Yaw, 0.f);
		const FVector DireccionAdelanteDeRotacionGiro(FRotationMatrix(RotacionGiro).GetUnitAxis(EAxis::X));
		AddMovementInput(DireccionAdelanteDeRotacionGiro, Valor);
	}
}

void ADispareitorPersonaje::MoverIzquierdaDerecha(float Valor) {
	if(Controller != nullptr && Valor != 0.f) {
		const FRotator RotacionGiro(0.f, Controller->GetControlRotation().Yaw, 0.f);
		const FVector DireccionLateralDeRotacionGiro(FRotationMatrix(RotacionGiro).GetUnitAxis(EAxis::Y));
		AddMovementInput(DireccionLateralDeRotacionGiro, Valor);
	}
}

void ADispareitorPersonaje::Girar(float Valor) {
	AddControllerYawInput(Valor);
}

void ADispareitorPersonaje::MoverArribaAbajo(float Valor) {
	AddControllerPitchInput(Valor);
}

