#include "Arma.h"
#include "Components/SphereComponent.h"

AArma::AArma() {
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true; // Para que nuestra arma tenga autoridad solo en el server

	Malla = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Malla"));
	Malla->SetupAttachment(RootComponent);
	SetRootComponent(Malla);	
	Malla->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	Malla->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	Malla->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	Esfera = CreateDefaultSubobject<USphereComponent>(TEXT("Esfera"));
	Esfera->SetupAttachment(RootComponent);
	Esfera->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	Esfera->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AArma::BeginPlay() {
	Super::BeginPlay();	

	if(HasAuthority()) { // Somos el servidor. Es lo mismo que GetLocalRole() == ENetRole::ROLE_Authority
		Esfera->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		Esfera->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	}
}

void AArma::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}

