#include "Arma.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Dispareitor/Personaje/DispareitorPersonaje.h"

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

void AArma::BeginPlay() {
	Super::BeginPlay();	
	
	if(HasAuthority()) { // Somos el servidor. Es lo mismo que GetLocalRole() == ENetRole::ROLE_Authority
		Esfera->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		Esfera->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
		Esfera->OnComponentBeginOverlap.AddDynamic(this, &AArma::CallbackEsferaSolapada);		
	}

	if(LeyendaSobreArma) {
		LeyendaSobreArma->SetVisibility(false);
	}
}

void AArma::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}

void AArma::CallbackEsferaSolapada(UPrimitiveComponent* ComponenteSolapado, AActor* OtroActor, UPrimitiveComponent* OtroComponente, int32 OtroIndice, bool bFromSweep, const FHitResult& SweepResult) { 
	ADispareitorPersonaje* DispareitorPersonaje = Cast<ADispareitorPersonaje>(OtroActor);
	if(DispareitorPersonaje && LeyendaSobreArma) {
		LeyendaSobreArma->SetVisibility(true);
	}
}

