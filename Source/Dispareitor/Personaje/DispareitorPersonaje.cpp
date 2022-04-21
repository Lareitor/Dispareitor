#include "DispareitorPersonaje.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"


ADispareitorPersonaje::ADispareitorPersonaje() {
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BrazoCamara = CreateDefaultSubobject<USpringArmComponent>(TEXT("BrazoCamara"));
	BrazoCamara->SetupAttachment(GetMesh());
	BrazoCamara->TargetArmLength = 600.f;
	BrazoCamara->bUsePawnControlRotation = true; 

	Camara = CreateDefaultSubobject<UCameraComponent>(TEXT("Camara"));
	Camara->SetupAttachment(BrazoCamara, USpringArmComponent::SocketName);
	Camara->bUsePawnControlRotation = false;
}

void ADispareitorPersonaje::BeginPlay() {
	Super::BeginPlay();	
}

void ADispareitorPersonaje::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ADispareitorPersonaje::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) {
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

