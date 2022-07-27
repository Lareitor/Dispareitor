#include "Casquillo.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"

ACasquillo::ACasquillo() {
	PrimaryActorTick.bCanEverTick = false;

	Malla = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Malla"));
	SetRootComponent(Malla);
	Malla->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	Malla->SetSimulatePhysics(true);
	Malla->SetEnableGravity(true);
	//Para que se generen eventos al golpear algo, en el BP se llama Simulation Generates Hit Events
	Malla->SetNotifyRigidBodyCollision(true);
	ImpulsoEyeccion = 10.f;
}

void ACasquillo::BeginPlay() {
	Super::BeginPlay();
	Malla->OnComponentHit.AddDynamic(this, &ACasquillo::Callback_AlGolpear);
	Malla->AddImpulse(GetActorForwardVector() * ImpulsoEyeccion);

	SetLifeSpan(3.f);
}

void ACasquillo::Callback_AlGolpear(UPrimitiveComponent* ComponenteGolpeante, AActor* ActorGolpeado, UPrimitiveComponent* ComponenteGolpeado, FVector ImpulsoNormal, const FHitResult& GolpeResultado) {
	if(Sonido) {
		UGameplayStatics::PlaySoundAtLocation(this, Sonido, GetActorLocation());
	}
	
	// Desactivar las siguientes colisiones contra el suelo para que no generen mas sonidos
	Malla->SetNotifyRigidBodyCollision(false);
}


