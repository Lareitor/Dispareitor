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

AArma::AArma() {
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true; // Para que nuestra arma tenga autoridad solo en el server

	Malla = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Malla"));
	SetRootComponent(Malla);	
	Malla->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	Malla->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	Malla->SetCollisionEnabled(ECollisionEnabled::NoCollision);

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
	
	if(HasAuthority()) { // Somos el servidor. Es lo mismo que GetLocalRole() == ENetRole::ROLE_Authority
		Esfera->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		Esfera->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
		Esfera->OnComponentBeginOverlap.AddDynamic(this, &AArma::CallbackEsferaSolapadaInicio);		
		Esfera->OnComponentEndOverlap.AddDynamic(this, &AArma::CallbackEsferaSolapadaFin);
	}

	if(LeyendaSobreArma) {
		LeyendaSobreArma->SetVisibility(false);
	}
}

void AArma::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AArma, Estado);
	DOREPLIFETIME(AArma, Municion);
}


void AArma::CallbackEsferaSolapadaInicio(UPrimitiveComponent* ComponenteSolapado, AActor* OtroActor, UPrimitiveComponent* OtroComponente, int32 OtroIndice, bool bFromSweep, const FHitResult& SweepResult) { 
	ADispareitorPersonaje* DispareitorPersonajeEntraEnEsfera = Cast<ADispareitorPersonaje>(OtroActor);
	if(DispareitorPersonajeEntraEnEsfera) {
		DispareitorPersonajeEntraEnEsfera->ActivarArmaSolapada(this);
	}
}

void AArma::CallbackEsferaSolapadaFin(UPrimitiveComponent* ComponenteSolapado, AActor* OtroActor, UPrimitiveComponent* OtroComponente, int32 OtroIndice) {
	ADispareitorPersonaje* DispareitorPersonajeSaleEsfera = Cast<ADispareitorPersonaje>(OtroActor);
	if(DispareitorPersonajeSaleEsfera) {
		DispareitorPersonajeSaleEsfera->ActivarArmaSolapada(nullptr);
	}
}

// Llamado por Soltar, UCombateComponente::EquiparArma, UCombateComponente::AlReplicarArmaEquipada
void AArma::ActualizarEstado(EEstado EstadoAActualizar) {
	Estado = EstadoAActualizar;

	switch(Estado) {
		case EEstado::EEA_Equipada:
			MostrarLeyendaSobreArma(false);
			Esfera->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			Malla->SetSimulatePhysics(false);
			Malla->SetEnableGravity(false);
			Malla->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		break;
		case EEstado::EEA_Desequipada:
			if(HasAuthority()) {
				Esfera->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
			}
			Malla->SetSimulatePhysics(true);
			Malla->SetEnableGravity(true);
			Malla->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		break;
	}	
}

void AArma::AlReplicarEstado() {
	switch(Estado) {
		case EEstado::EEA_Equipada:
			MostrarLeyendaSobreArma(false);	
			Malla->SetSimulatePhysics(false);
			Malla->SetEnableGravity(false);
			Malla->SetCollisionEnabled(ECollisionEnabled::NoCollision);		
		break;
		case EEstado::EEA_Desequipada:
			Malla->SetSimulatePhysics(true);
			Malla->SetEnableGravity(true);
			Malla->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		break;
	}
}

void AArma::MostrarLeyendaSobreArma(bool bMostrarLeyendaSobreArma) {
	if(LeyendaSobreArma) {
		LeyendaSobreArma->SetVisibility(bMostrarLeyendaSobreArma);
	}
}

// Llamado por UCombateComponente::MulticastDisparar_Implementation y por sus clases hijas
void AArma::Disparar(const FVector& Objetivo) {
	if(AnimacionDisparar) {
		Malla->PlayAnimation(AnimacionDisparar, false);
	}
	if(Casquillo) {
		const USkeletalMeshSocket* CasquilloSocket = Malla->GetSocketByName(FName("AmmoEject"));
		if(CasquilloSocket) {
			FTransform CasquilloSocketTransform = CasquilloSocket->GetSocketTransform(Malla);
			UWorld* Mundo = GetWorld();
			if(Mundo) {
				Mundo->SpawnActor<ACasquillo>(Casquillo, CasquilloSocketTransform.GetLocation(), CasquilloSocketTransform.GetRotation().Rotator());
			}
		}
	}

	GastarMunicion();
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
	Municion = FMath::Clamp(Municion -1, 0, CargadorCapacidad);
	ActualizarHUDMunicion();
}

void AArma::AlReplicarMunicion() {
	ActualizarHUDMunicion();
}

void AArma::OnRep_Owner() {
	Super::OnRep_Owner();
	if(Owner == nullptr) {
		DispareitorPersonaje = nullptr;
		DispareitorControladorJugador = nullptr;
	} else {
		ActualizarHUDMunicion();
	}
}

void AArma::ActualizarHUDMunicion() {
	DispareitorPersonaje = DispareitorPersonaje != nullptr ? DispareitorPersonaje : Cast<ADispareitorPersonaje>(GetOwner());
	if(DispareitorPersonaje) {
		DispareitorControladorJugador = DispareitorControladorJugador != nullptr ? DispareitorControladorJugador : Cast<ADispareitorControladorJugador>(DispareitorPersonaje->Controller);
		if(DispareitorControladorJugador) {
			DispareitorControladorJugador->ActualizarHUDMunicionArma(Municion);
		}
	}
}

bool AArma::EstaSinMunicion() {
	return Municion <= 0;
}

// Llamado por UCombateComponente::RecargarServidor_Implementation
void AArma::MunicionModificar(int32 Cantidad) {
	Municion = FMath::Clamp(Municion + Cantidad, 0, CargadorCapacidad);
	ActualizarHUDMunicion();
}
