#include "Pickup.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Components/SphereComponent.h"
#include "Dispareitor/Tipos/TiposArma.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"

APickup::APickup() {
 	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	//SceneComponent nos permite poder anclar otros componentes relativos a él, y como scenecomponent se coloca en la superficie del mapa, podemos por ejemplo colocarlos con un offset un poco mas alto para 
	//ahorrarnos el tener que subirlos manualmente 
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("ComponenteRaiz"));
	ComponenteEsfera = CreateDefaultSubobject<USphereComponent>(TEXT("ComponenteEsfera"));
	ComponenteEsfera->SetupAttachment(RootComponent);
	ComponenteEsfera->SetSphereRadius(80.f);
	ComponenteEsfera->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	ComponenteEsfera->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	ComponenteEsfera->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	ComponenteEsfera->AddLocalOffset(FVector(0.f, 0.f, 70.f));

	ComponenteMallaEstatica = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ComponenteMallaEstatica"));
	ComponenteMallaEstatica->SetupAttachment(ComponenteEsfera);
	ComponenteMallaEstatica->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ComponenteMallaEstatica->SetRelativeScale3D(FVector(2.f, 2.f, 2.f));
	ComponenteMallaEstatica->SetRenderCustomDepth(true);
	ComponenteMallaEstatica->SetCustomDepthStencilValue(PROFUNDIDAD_PERSONALIZADA_AL_RENDERIZAR_MORADO);

	ComponenteNiagaraFX = CreateDefaultSubobject<UNiagaraComponent>(TEXT("ComponenteNiagaraFX"));
    ComponenteNiagaraFX->SetupAttachment(RootComponent);
}

void APickup::BeginPlay(){
	Super::BeginPlay();

	if(HasAuthority()) {
		ComponenteEsfera->OnComponentBeginOverlap.AddDynamic(this, &APickup::Callback_ComponenteEsferaSolapadaInicio);
	}
}

void APickup::Tick(float DeltaTime){
	Super::Tick(DeltaTime);
	if(ComponenteMallaEstatica) {
		ComponenteMallaEstatica->AddWorldRotation(FRotator(0.f, RatioDeGiro * DeltaTime, 0.f));
	}
}

void APickup::Callback_ComponenteEsferaSolapadaInicio(UPrimitiveComponent* ComponenteSolapado, AActor* OtroActor, UPrimitiveComponent* OtroComponente, int32 OtroIndice, bool bFromSweep, const FHitResult& SweepResult) {

}

void APickup::Destroyed() {
	Super::Destroyed();

	if(Sonido) {
		UGameplayStatics::PlaySoundAtLocation(this, Sonido, GetActorLocation());
	}
	if(SistemaNiagaraFX) {
        UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, SistemaNiagaraFX, GetActorLocation(), GetActorRotation());
    }
}
	

