#include "Arma.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Dispareitor/Personaje/DispareitorPersonaje.h"
#include "Net/UnrealNetwork.h"
#include "Animation/AnimationAsset.h"
#include "Components/SkeletalMeshComponent.h" 
#include "Casquillo.h"
#include "Engine/SkeletalMeshSocket.h"

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
}


void AArma::CallbackEsferaSolapadaInicio(UPrimitiveComponent* ComponenteSolapado, AActor* OtroActor, UPrimitiveComponent* OtroComponente, int32 OtroIndice, bool bFromSweep, const FHitResult& SweepResult) { 
	ADispareitorPersonaje* DispareitorPersonaje = Cast<ADispareitorPersonaje>(OtroActor);
	if(DispareitorPersonaje) {
		DispareitorPersonaje->ActivarArmaSolapada(this);
	}
}

void AArma::CallbackEsferaSolapadaFin(UPrimitiveComponent* ComponenteSolapado, AActor* OtroActor, UPrimitiveComponent* OtroComponente, int32 OtroIndice) {
	ADispareitorPersonaje* DispareitorPersonaje = Cast<ADispareitorPersonaje>(OtroActor);
	if(DispareitorPersonaje) {
		DispareitorPersonaje->ActivarArmaSolapada(nullptr);
	}
}

void AArma::ActualizarEstado(EEstado EstadoAActualizar) {
	Estado = EstadoAActualizar;

	switch(Estado) {
		case EEstado::EEA_Equipada:
			MostrarLeyendaSobreArma(false);
			Esfera->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		break;
	}	
}

void AArma::AlReplicarEstado() {
	switch(Estado) {
		case EEstado::EEA_Equipada:
			MostrarLeyendaSobreArma(false);			
		break;
	}
}

void AArma::MostrarLeyendaSobreArma(bool bMostrarLeyendaSobreArma) {
	if(LeyendaSobreArma) {
		LeyendaSobreArma->SetVisibility(bMostrarLeyendaSobreArma);
	}
}

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
}
