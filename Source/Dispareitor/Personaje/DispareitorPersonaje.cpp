#include "DispareitorPersonaje.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/WidgetComponent.h"
#include "Net/UnrealNetwork.h"
#include "Dispareitor/Arma/Arma.h"
#include "Dispareitor/DispareitorComponentes/CombateComponente.h"


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

	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true; 

	HUDSobreLaCabeza = CreateDefaultSubobject<UWidgetComponent>(TEXT("HUDSobreLaCabeza"));
	HUDSobreLaCabeza->SetupAttachment(RootComponent);

	CombateComponente = CreateDefaultSubobject<UCombateComponente>(TEXT("CombateComponente"));
	// No necesitamos registrar los componentes en GetLifetimeReplicatedProps, solo necesitamos activarles esta propiedad
	CombateComponente->SetIsReplicated(true);
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
	PlayerInputComponent->BindAction("Equipar", IE_Pressed, this, &ADispareitorPersonaje::Equipar);

	PlayerInputComponent->BindAxis("MoverAdelanteAtras", this, &ADispareitorPersonaje::MoverAdelanteAtras);
	PlayerInputComponent->BindAxis("MoverIzquierdaDerecha", this, &ADispareitorPersonaje::MoverIzquierdaDerecha);
	PlayerInputComponent->BindAxis("Girar", this, &ADispareitorPersonaje::Girar);
	PlayerInputComponent->BindAxis("MirarArribaAbajo", this, &ADispareitorPersonaje::MirarArribaAbajo);
}

void ADispareitorPersonaje::PostInitializeComponents() {
	Super::PostInitializeComponents();
	if(CombateComponente) {
		CombateComponente->DispareitorPersonaje = this;
	}
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

void ADispareitorPersonaje::MirarArribaAbajo(float Valor) {
	AddControllerPitchInput(Valor);
}

void ADispareitorPersonaje::Equipar() {
	if(CombateComponente) {
		if(HasAuthority()) { // Estamos en el servidor
			CombateComponente->EquiparArma(ArmaSolapada);
		} else { // Estamos en un cliente
			ServidorEquipar();
		}		
	}
}

// Aunque la definicion de la funcion es ServidorEquipar hay que añadirle _Implementation, ya que UE creará ServidorEquipar y nosotros _Implementation que incluirá el codigo que se ejecuta en el servidor  
void ADispareitorPersonaje::ServidorEquipar_Implementation() {
	if(CombateComponente) {
		CombateComponente->EquiparArma(ArmaSolapada);
	}
}

// En esta funcion es donde registramos las variables que queremos replicar
void ADispareitorPersonaje::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Solo se replica el ArmaSolapada en el cliente que posee ADispareitorPersonaje
	DOREPLIFETIME_CONDITION(ADispareitorPersonaje, ArmaSolapada, COND_OwnerOnly);
}

//Para el servidor
//Sabemos que esta funcion solo es llamada en el servidor 
void ADispareitorPersonaje::ActivarArmaSolapada(AArma* Arma) {
	if(IsLocallyControlled()) { 
		if(ArmaSolapada) {
			ArmaSolapada->MostrarLeyendaSobreArma(false);
		}
	}

	ArmaSolapada = Arma;	
	if(IsLocallyControlled()) { // y ademas somos el jugador que está hospedando el juego
		if(ArmaSolapada) {
			ArmaSolapada->MostrarLeyendaSobreArma(true);
		}
	}
}

// Para los clientes
// Se llama automaticamente en el cliente cuando la variable es replicada por el servidor. Nunca se llama en el servidor
// Acepta 0 ó 1 argumento. Si le pasamos argumento tiene que ser del tipo de la variable replicada, y se rellenará con el valor anterior replicado o null si no lo tuviera
void ADispareitorPersonaje::AlReplicarArmaSolapada(AArma* ArmaReplicadaAnterior) {
	if(ArmaSolapada) {
		ArmaSolapada->MostrarLeyendaSobreArma(true);
	}
	if(ArmaReplicadaAnterior) {
		ArmaReplicadaAnterior->MostrarLeyendaSobreArma(false);
	}
}

