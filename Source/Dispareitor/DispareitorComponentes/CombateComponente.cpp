#include "CombateComponente.h"
#include "Dispareitor/Arma/Arma.h"
#include "Dispareitor/Personaje/DispareitorPersonaje.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/CharacterMovementComponent.h"

UCombateComponente::UCombateComponente() {
	PrimaryComponentTick.bCanEverTick = false;
}

void UCombateComponente::BeginPlay() {
	Super::BeginPlay();	
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
	
}

void UCombateComponente::ServidorActualizarApuntando_Implementation(bool Apuntando) {
	bApuntando = Apuntando; 
}
 
void UCombateComponente::AlReplicarArmaEquipada() {
	if(ArmaEquipada && DispareitorPersonaje) {
		DispareitorPersonaje->GetCharacterMovement()->bOrientRotationToMovement = false;
		DispareitorPersonaje->bUseControllerRotationYaw = true;
	}
}

