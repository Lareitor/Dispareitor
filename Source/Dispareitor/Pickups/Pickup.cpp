#include "Pickup.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Components/SphereComponent.h"

APickup::APickup() {
 	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	//SceneComponent nos permite poder anclar otros componentes relativos a él, y como scenecomponent se coloca en la superficie del mapa, podemos por ejemplo colocarlos con un offset un poco mas alto para 
	//ahorranos el tener que subirlos manualmente 
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("ComponenteRaiz"));
	ComponenteEsfera = CreateDefaultSubobject<USphereComponent>(TEXT("ComponenteEsfera"));
	ComponenteEsfera->SetupAttachment(RootComponent);
	ComponenteEsfera->SetSphereRadius(80.f);
	ComponenteEsfera->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	ComponenteEsfera->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	ComponenteEsfera->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	ComponenteMallaEstatica = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ComponenteMallaEstatica"));
	ComponenteMallaEstatica->SetupAttachment(ComponenteEsfera);
	ComponenteMallaEstatica->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void APickup::BeginPlay(){
	Super::BeginPlay();

	if(HasAuthority()) {
		ComponenteEsfera->OnComponentBeginOverlap.AddDynamic(this, &APickup::Callback_ComponenteEsferaSolapadaInicio);
	}
}

void APickup::Tick(float DeltaTime){
	Super::Tick(DeltaTime);
}

void APickup::Callback_ComponenteEsferaSolapadaInicio(UPrimitiveComponent* ComponenteSolapado, AActor* OtroActor, UPrimitiveComponent* OtroComponente, int32 OtroIndice, bool bFromSweep, const FHitResult& SweepResult) {

}

void APickup::Destroyed() {
	Super::Destroyed();

	if(Sonido) {
		UGameplayStatics::PlaySoundAtLocation(this, Sonido, GetActorLocation());
	}
}
	

