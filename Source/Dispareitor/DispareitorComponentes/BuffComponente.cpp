#include "BuffComponente.h"
#include "Dispareitor/Personaje/DispareitorPersonaje.h"
#include "GameFramework/CharacterMovementComponent.h"

UBuffComponente::UBuffComponente() {
	PrimaryComponentTick.bCanEverTick = true;
}

void UBuffComponente::BeginPlay() {
	Super::BeginPlay();
	
}

void UBuffComponente::InicializarVelocidadesOriginales(float _VelocidadDePieOriginal, float _VelocidadAgachadoOriginal) {
	VelocidadDePieOriginal = _VelocidadDePieOriginal;
	VelocidadAgachadoOriginal = _VelocidadAgachadoOriginal;
}

void UBuffComponente::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	SanarProgreso(DeltaTime);
}

void UBuffComponente::Sanar(float IncrementoVida, float TiempoIncrementoVida) {
	bSanando = true;
	RatioSanacion = IncrementoVida / TiempoIncrementoVida;
	IncrementoASanar += IncrementoVida;
}

void UBuffComponente::SanarProgreso(float DeltaTime) {
	if(!bSanando || DispareitorPersonaje == nullptr || DispareitorPersonaje->EstaEliminado()) {
		return;
	}

	const float SanacionEsteFrame = RatioSanacion * DeltaTime;
	DispareitorPersonaje->ActualizarVida(FMath::Clamp(DispareitorPersonaje->ObtenerVida() + SanacionEsteFrame, 0, DispareitorPersonaje->ObtenerVidaMaxima()));
	DispareitorPersonaje->ActualizarVidaHUD();
	IncrementoASanar -=SanacionEsteFrame;
	
	if(IncrementoASanar <= 0.f || DispareitorPersonaje->ObtenerVida() >= DispareitorPersonaje->ObtenerVidaMaxima()) {
		bSanando = false;
		IncrementoASanar = 0.f;
	}
}

void UBuffComponente::AumentarVelocidad(float VelocidadDePieAumentada, float VelocidadAgachadoAumentada, float Duracion) {
	if(DispareitorPersonaje == nullptr || DispareitorPersonaje->GetCharacterMovement() == nullptr) {
		return;
	}

	DispareitorPersonaje->GetWorldTimerManager().SetTimer(TemporizadorAumentoVelocidad, this, &UBuffComponente::ResetearVelocidades, Duracion);
	DispareitorPersonaje->GetCharacterMovement()->MaxWalkSpeed = VelocidadDePieAumentada;
	DispareitorPersonaje->GetCharacterMovement()->MaxWalkSpeedCrouched = VelocidadAgachadoAumentada;
	AumentarVelocidad_Multicast(VelocidadDePieAumentada, VelocidadAgachadoAumentada);
}


void UBuffComponente::ResetearVelocidades() {
	if(DispareitorPersonaje == nullptr || DispareitorPersonaje->GetCharacterMovement() == nullptr) {
		return;
	}

	DispareitorPersonaje->GetCharacterMovement()->MaxWalkSpeed = VelocidadDePieOriginal;
	DispareitorPersonaje->GetCharacterMovement()->MaxWalkSpeedCrouched = VelocidadAgachadoOriginal;
	AumentarVelocidad_Multicast(VelocidadDePieOriginal, VelocidadAgachadoOriginal);
}

void UBuffComponente::AumentarVelocidad_Multicast_Implementation(float VelocidadDePieNueva, float VelocidadAgachadoNueva) {	
	DispareitorPersonaje->GetCharacterMovement()->MaxWalkSpeed = VelocidadDePieNueva;
	DispareitorPersonaje->GetCharacterMovement()->MaxWalkSpeedCrouched = VelocidadAgachadoNueva;

}

