#include "CombateComponente.h"
#include "Dispareitor/Arma/Arma.h"
#include "Dispareitor/Personaje/DispareitorPersonaje.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/CharacterMovementComponent.h"

UCombateComponente::UCombateComponente() {
	PrimaryComponentTick.bCanEverTick = false;

	VelocidadCaminarBase = 600.f;
	VelocidadCaminarApuntando = 400.f;
}

void UCombateComponente::BeginPlay() {
	Super::BeginPlay();	

	if(DispareitorPersonaje) {
		DispareitorPersonaje->GetCharacterMovement()->MaxWalkSpeed = VelocidadCaminarBase;
	}
}

void UCombateComponente::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UCombateComponente::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UCombateComponente, ArmaEquipada);
	DOREPLIFETIME(UCombateComponente, bApuntando);
}

void UCombateComponente::EquiparArma(class AArma* ArmaAEquipar) {
	if(DispareitorPersonaje == nullptr || ArmaAEquipar == nullptr) {
		return;
	}

	ArmaEquipada = ArmaAEquipar;
	ArmaEquipada->ActualizarEstado(EEstado::EEA_Equipada);
	const USkeletalMeshSocket* ManoDerechaSocket = DispareitorPersonaje->GetMesh()->GetSocketByName(FName("ManoDerechaSocket"));
	if(ManoDerechaSocket) {
		ManoDerechaSocket->AttachActor(ArmaEquipada, DispareitorPersonaje->GetMesh());
	}
	ArmaEquipada->SetOwner(DispareitorPersonaje);	
	DispareitorPersonaje->GetCharacterMovement()->bOrientRotationToMovement = false;
	DispareitorPersonaje->bUseControllerRotationYaw = true;
}

void UCombateComponente::ActualizarApuntando(bool Apuntando) {
	// Aunque esta funcion es posible que la llamemos desde el cliente, por cuestiones cosmeticas podemos hacerlo ahora
	bApuntando = Apuntando; 
	// Si lo estamos ejecutando en un cliente se invocará esta función en el servidor, y si lo estamos ejecutando en el servidor se ejecutará en él mismo
	ServidorActualizarApuntando(bApuntando);

	if(DispareitorPersonaje) {
		DispareitorPersonaje->GetCharacterMovement()->MaxWalkSpeed = bApuntando ? VelocidadCaminarApuntando : VelocidadCaminarBase;
	}
	
}

void UCombateComponente::ServidorActualizarApuntando_Implementation(bool Apuntando) {
	bApuntando = Apuntando; 
	if(DispareitorPersonaje) {
		DispareitorPersonaje->GetCharacterMovement()->MaxWalkSpeed = bApuntando ? VelocidadCaminarApuntando : VelocidadCaminarBase;
	} 
}
 
void UCombateComponente::AlReplicarArmaEquipada() {
	if(ArmaEquipada && DispareitorPersonaje) {
		DispareitorPersonaje->GetCharacterMovement()->bOrientRotationToMovement = false;
		DispareitorPersonaje->bUseControllerRotationYaw = true;
	}
}

void UCombateComponente::DispararPresionado(bool bPresionado) {
	bDispararPresionado = bPresionado;
	if(bDispararPresionado) {
		// Si estamos en el server se ejecutar en el server y se estamos en un cliente se ejectura en el server
		ServidorDisparar();
	}	
}

// Esta funcion solo se ejecutará en el servidor
void UCombateComponente::ServidorDisparar_Implementation() {
	MulticastDisparar();
}

// Esta función se ejecutará en el servidor + clientes
void UCombateComponente::MulticastDisparar_Implementation() {
	if(DispareitorPersonaje && ArmaEquipada) {
		DispareitorPersonaje->EjecutarMontajeDispararArma(bApuntando);
		ArmaEquipada->Disparar();
	}
}
