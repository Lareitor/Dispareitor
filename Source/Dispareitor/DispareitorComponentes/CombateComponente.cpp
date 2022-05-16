#include "CombateComponente.h"
#include "Dispareitor/Arma/Arma.h"
#include "Dispareitor/Personaje/DispareitorPersonaje.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "Dispareitor/ControladorJugador/DispareitorControladorJugador.h"
#include "Dispareitor/HUD/DispareitorHUD.h"

UCombateComponente::UCombateComponente() {
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(true);

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
	
	ActualizarHUDCruceta(DeltaTime);
}

void UCombateComponente::ActualizarHUDCruceta(float DeltaTime) {
	if(DispareitorPersonaje == nullptr || DispareitorPersonaje->Controller == nullptr) {
		return;
	}

	DispareitorControladorJugador = DispareitorControladorJugador == nullptr ? Cast<ADispareitorControladorJugador>(DispareitorPersonaje->Controller) : DispareitorControladorJugador; 
	if(DispareitorControladorJugador) {
		DispareitorHUD = DispareitorHUD == nullptr ? Cast<ADispareitorHUD>(DispareitorControladorJugador->GetHUD()) : DispareitorHUD;
		if(DispareitorHUD) {
			FHUDTexturas HUDTexturas;
			if(ArmaEquipada) {
				HUDTexturas.CrucetaCentro = ArmaEquipada->CrucetaCentro;
				HUDTexturas.CrucetaIzquierda = ArmaEquipada->CrucetaIzquierda;
				HUDTexturas.CrucetaDerecha = ArmaEquipada->CrucetaDerecha;
				HUDTexturas.CrucetaArriba = ArmaEquipada->CrucetaArriba;
				HUDTexturas.CrucetaAbajo = ArmaEquipada->CrucetaAbajo;
			} else {
				HUDTexturas.CrucetaCentro = nullptr;
				HUDTexturas.CrucetaIzquierda = nullptr;
				HUDTexturas.CrucetaDerecha = nullptr;
				HUDTexturas.CrucetaArriba = nullptr;
				HUDTexturas.CrucetaAbajo = nullptr;
			}
			DispareitorHUD->ActualizarHUDTexturas(HUDTexturas);
		}
	}
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
		FHitResult RayoResultado;
		CrucetaRayo(RayoResultado);

		// Si estamos en el server se ejecutar en el server y si estamos en un cliente se ejectura en el server
		ServidorDisparar(RayoResultado.ImpactPoint);
	}	
}

// Esta funcion solo se ejecutará en el servidor
void UCombateComponente::ServidorDisparar_Implementation(const FVector_NetQuantize& Objetivo) {
	MulticastDisparar(Objetivo);
}

// Esta función se ejecutará en el servidor + clientes
void UCombateComponente::MulticastDisparar_Implementation(const FVector_NetQuantize& Objetivo) {
	if(DispareitorPersonaje && ArmaEquipada) {
		DispareitorPersonaje->EjecutarMontajeDispararArma(bApuntando);
		ArmaEquipada->Disparar(Objetivo);
	}
}

void UCombateComponente::CrucetaRayo(FHitResult& RayoResultado) {
	FVector2D PantallaTamano;
	if(GEngine && GEngine->GameViewport) {
		GEngine->GameViewport->GetViewportSize(PantallaTamano);
	}

	FVector2D CrucetaLocalizacion(PantallaTamano.X / 2.f, PantallaTamano.Y / 2.f);
	FVector CrucetaMundoPosicion;
	FVector CrucetaMundoDireccion;
	bool bPantallaAMundo = UGameplayStatics::DeprojectScreenToWorld(UGameplayStatics::GetPlayerController(this, 0), CrucetaLocalizacion, CrucetaMundoPosicion, CrucetaMundoDireccion);
	if(bPantallaAMundo) {
		FVector Inicio = CrucetaMundoPosicion;
		FVector Fin = Inicio + CrucetaMundoDireccion * RAYO_LONGITUD;
		GetWorld()->LineTraceSingleByChannel(RayoResultado, Inicio, Fin, ECollisionChannel::ECC_Visibility);
	}
}

