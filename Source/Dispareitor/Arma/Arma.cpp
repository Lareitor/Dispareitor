#include "Arma.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Dispareitor/Personaje/DispareitorPersonaje.h"
#include "Net/UnrealNetwork.h"
#include "Animation/AnimationAsset.h"
#include "Components/SkeletalMeshComponent.h" 
#include "Casquillo.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Dispareitor/ControladorJugador/DispareitorControladorJugador.h"
#include "Dispareitor/DispareitorComponentes/CombateComponente.h"

AArma::AArma() {
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true; // Para que nuestra arma tenga autoridad solo en el server
	SetReplicateMovement(true);

	Malla = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Malla"));
	SetRootComponent(Malla);	
	Malla->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	Malla->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	Malla->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	Malla->SetCustomDepthStencilValue(PROFUNDIDAD_PERSONALIZADA_AL_RENDERIZAR_AZUL);
	Malla->MarkRenderStateDirty(); // Para obligar a refrescar el render
	PermitirProfundidadPersonalizadaAlRenderizar(true);

	Esfera = CreateDefaultSubobject<USphereComponent>(TEXT("Esfera"));
	Esfera->SetupAttachment(RootComponent);
	Esfera->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	Esfera->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	LeyendaSobreArma = CreateDefaultSubobject<UWidgetComponent>(TEXT("LeyendaSobreArma"));
	LeyendaSobreArma->SetupAttachment(RootComponent);
}

void AArma::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}

void AArma::BeginPlay() {
	Super::BeginPlay();	
	
	Esfera->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Esfera->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	Esfera->OnComponentBeginOverlap.AddDynamic(this, &AArma::Callback_EsferaSolapadaInicio);		
	Esfera->OnComponentEndOverlap.AddDynamic(this, &AArma::Callback_EsferaSolapadaFin);

	if(LeyendaSobreArma) {
		LeyendaSobreArma->SetVisibility(false);
	}
}

void AArma::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AArma, Estado);
	DOREPLIFETIME(AArma, Municion);
}


void AArma::Callback_EsferaSolapadaInicio(UPrimitiveComponent* ComponenteSolapado, AActor* OtroActor, UPrimitiveComponent* OtroComponente, int32 OtroIndice, bool bFromSweep, const FHitResult& SweepResult) { 
	ADispareitorPersonaje* DispareitorPersonajeEntraEnEsfera = Cast<ADispareitorPersonaje>(OtroActor);
	if(DispareitorPersonajeEntraEnEsfera) {
		DispareitorPersonajeEntraEnEsfera->ActivarArmaSolapada(this);
	}
}

void AArma::Callback_EsferaSolapadaFin(UPrimitiveComponent* ComponenteSolapado, AActor* OtroActor, UPrimitiveComponent* OtroComponente, int32 OtroIndice) {
	ADispareitorPersonaje* DispareitorPersonajeSaleEsfera = Cast<ADispareitorPersonaje>(OtroActor);
	if(DispareitorPersonajeSaleEsfera) {
		DispareitorPersonajeSaleEsfera->ActivarArmaSolapada(nullptr);
	}
}

// Llamado por Soltar, UCombateComponente::EquiparArma, UCombateComponente::AlReplicar_ArmaEquipada
void AArma::ActualizarEstado(EEstado EstadoAActualizar) {
	Estado = EstadoAActualizar;
	ManejarActualizacionEstado();
}

void AArma::AlReplicar_Estado() {
	ManejarActualizacionEstado();
}

void AArma::ManejarActualizacionEstado() {
	switch(Estado) {
		case EEstado::EEA_Equipada:
			ManejarActualizacionEstadoAlEquipar();
		break;
		case EEstado::EEA_EquipadaSecundaria:
			ManejarActualizacionEstadoAlEquiparSecundaria();
		break;
		case EEstado::EEA_Desequipada:
			ManejarActualizacionEstadoAlSoltar();
		break;
	}	
}

void AArma::ManejarActualizacionEstadoAlEquipar() {
	MostrarLeyendaSobreArma(false);
	Esfera->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Malla->SetSimulatePhysics(false);
	Malla->SetEnableGravity(false);
	Malla->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	if(TipoArma == ETipoArma::ETA_Subfusil) { //Para permitir las fisicas en la correa
		Malla->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		Malla->SetEnableGravity(true);
		Malla->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	}
	PermitirProfundidadPersonalizadaAlRenderizar(false);
}

void AArma::ManejarActualizacionEstadoAlEquiparSecundaria() {
	MostrarLeyendaSobreArma(false);
	Esfera->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Malla->SetSimulatePhysics(false);
	Malla->SetEnableGravity(false);
	Malla->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	if(TipoArma == ETipoArma::ETA_Subfusil) { 
		Malla->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		Malla->SetEnableGravity(true);
		Malla->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	}
	PermitirProfundidadPersonalizadaAlRenderizar(false);
}

void AArma::ManejarActualizacionEstadoAlSoltar() {
	if(HasAuthority()) {
		Esfera->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	}
	Malla->SetSimulatePhysics(true);
	Malla->SetEnableGravity(true);
	Malla->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Malla->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	Malla->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	Malla->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
			
	Malla->SetCustomDepthStencilValue(PROFUNDIDAD_PERSONALIZADA_AL_RENDERIZAR_AZUL);
	Malla->MarkRenderStateDirty();
	PermitirProfundidadPersonalizadaAlRenderizar(true);	
}

void AArma::MostrarLeyendaSobreArma(bool bMostrarLeyendaSobreArma) {
	if(LeyendaSobreArma) {
		LeyendaSobreArma->SetVisibility(bMostrarLeyendaSobreArma);
	}
}

// Llamado por UCombateComponente::Disparar_Multicast_Implementation y por sus clases hijas
void AArma::Disparar(const FVector& Objetivo) {
	if(AnimacionDisparar) {
		Malla->PlayAnimation(AnimacionDisparar, false);
	}
	if(ClaseCasquillo) {
		const USkeletalMeshSocket* CasquilloSocket = Malla->GetSocketByName(FName("AmmoEject"));
		if(CasquilloSocket) {
			FTransform CasquilloSocketTransform = CasquilloSocket->GetSocketTransform(Malla);
			UWorld* Mundo = GetWorld();
			if(Mundo) {
				Mundo->SpawnActor<ACasquillo>(ClaseCasquillo, CasquilloSocketTransform.GetLocation(), CasquilloSocketTransform.GetRotation().Rotator());
			}
		}
	}

	if(HasAuthority()) {
		GastarMunicion();
	}
}

// Llamado por ADispareitorPersonaje::Eliminado
void AArma::Soltar() {
	ActualizarEstado(EEstado::EEA_Desequipada);
	FDetachmentTransformRules DesvincularReglas(EDetachmentRule::KeepWorld, true);
	Malla->DetachFromComponent(DesvincularReglas);
	SetOwner(nullptr);
	DispareitorPersonaje = nullptr;
	DispareitorControladorJugador = nullptr;
}

// Llamado por Disparar
void AArma::GastarMunicion() {
	Municion = FMath::Clamp(Municion -1, 0, CapacidadCargador);
	ActualizarMunicionHUD();
}

void AArma::AlReplicar_Municion() {
	DispareitorPersonaje = DispareitorPersonaje != nullptr ? DispareitorPersonaje : Cast<ADispareitorPersonaje>(GetOwner());
	if(DispareitorPersonaje && DispareitorPersonaje->ObtenerCombateComponente() && EstaConMunicionLlena()) {
		DispareitorPersonaje->ObtenerCombateComponente()->SaltarAFinAnimacionEscopeta();
	}

	ActualizarMunicionHUD();
}

void AArma::OnRep_Owner() {
	Super::OnRep_Owner();
	if(Owner == nullptr) {
		DispareitorPersonaje = nullptr;
		DispareitorControladorJugador = nullptr;
	} else {
		DispareitorPersonaje = DispareitorPersonaje == nullptr ? Cast<ADispareitorPersonaje>(Owner) : DispareitorPersonaje;
		if(DispareitorPersonaje && DispareitorPersonaje->ObtenerArmaEquipada() && DispareitorPersonaje->ObtenerArmaEquipada() == this) {
			ActualizarMunicionHUD();
		}
	}
}

void AArma::ActualizarMunicionHUD() {
	DispareitorPersonaje = DispareitorPersonaje != nullptr ? DispareitorPersonaje : Cast<ADispareitorPersonaje>(GetOwner());
	if(DispareitorPersonaje) {
		DispareitorControladorJugador = DispareitorControladorJugador != nullptr ? DispareitorControladorJugador : Cast<ADispareitorControladorJugador>(DispareitorPersonaje->Controller);
		if(DispareitorControladorJugador) {
			DispareitorControladorJugador->ActualizarMunicionArmaHUD(Municion);
		}
	}
}

bool AArma::EstaSinMunicion() {
	return Municion <= 0;
}

bool AArma::EstaConMunicionLlena() {
	return Municion == CapacidadCargador;
}

// Llamado por UCombateComponente::ActualizarValoresMunicion
void AArma::AniadirMunicion(int32 Cantidad) {
	Municion = FMath::Clamp(Municion + Cantidad, 0, CapacidadCargador);
	ActualizarMunicionHUD();
}


// Habilitar o deshabilitar custom depth para el outline en las armas
void AArma::PermitirProfundidadPersonalizadaAlRenderizar(bool bPermitir) {
	if(Malla) {
		Malla->SetRenderCustomDepth(bPermitir);
	}
}
