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
	EscudarProgreso(DeltaTime);
}

void UBuffComponente::Sanar(float IncrementoVida, float TiempoIncrementoVida) {
	bSanando = true;
	RatioSanacion = IncrementoVida / TiempoIncrementoVida;
	IncrementoASanar += IncrementoVida;
}

void UBuffComponente::Escudar(float IncrementoEscudo, float TiempoIncrementoEscudo) {
	bEscudando = true;
	RatioEscudacion = IncrementoEscudo / TiempoIncrementoEscudo;
	IncrementoAEscudar += IncrementoEscudo;
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

void UBuffComponente::EscudarProgreso(float DeltaTime) {
	if(!bEscudando || DispareitorPersonaje == nullptr || DispareitorPersonaje->EstaEliminado()) {
		return;
	}

	const float EscudacionEsteFrame = RatioEscudacion * DeltaTime;
	DispareitorPersonaje->ActualizarEscudo(FMath::Clamp(DispareitorPersonaje->ObtenerEscudo() + EscudacionEsteFrame, 0, DispareitorPersonaje->ObtenerEscudoMaximo()));
	DispareitorPersonaje->ActualizarEscudoHUD();
	IncrementoAEscudar -=EscudacionEsteFrame;
	
	if(IncrementoAEscudar <= 0.f || DispareitorPersonaje->ObtenerEscudo() >= DispareitorPersonaje->ObtenerEscudoMaximo()) {
		bEscudando = false;
		IncrementoAEscudar = 0.f;
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
	if(DispareitorPersonaje == nullptr || DispareitorPersonaje->GetCharacterMovement() == nullptr) {
		return;
	}
	DispareitorPersonaje->GetCharacterMovement()->MaxWalkSpeed = VelocidadDePieNueva;
	DispareitorPersonaje->GetCharacterMovement()->MaxWalkSpeedCrouched = VelocidadAgachadoNueva;
}

void UBuffComponente::InicializarSaltoOriginal(float _SaltoOriginal) {
	SaltoOriginal = _SaltoOriginal;
}

void UBuffComponente::AumentarSalto(float SaltoAumentando, float Duracion) {
	if(DispareitorPersonaje == nullptr || DispareitorPersonaje->GetCharacterMovement() == nullptr) {
		return;
	}

	DispareitorPersonaje->GetWorldTimerManager().SetTimer(TemporizadorAumentoSalto, this, &UBuffComponente::ResetearSalto, Duracion);
	DispareitorPersonaje->GetCharacterMovement()->JumpZVelocity = SaltoAumentando;
	AumentarSalto_Multicast(SaltoAumentando);
}

void UBuffComponente::ResetearSalto() {
	if(DispareitorPersonaje == nullptr || DispareitorPersonaje->GetCharacterMovement() == nullptr) {
		return;
	}

	DispareitorPersonaje->GetCharacterMovement()->JumpZVelocity = SaltoOriginal;
	AumentarSalto_Multicast(SaltoOriginal);
}

void UBuffComponente::AumentarSalto_Multicast_Implementation(float SaltoNuevo) {
	if(DispareitorPersonaje == nullptr || DispareitorPersonaje->GetCharacterMovement() == nullptr) {
		return;
	}
	DispareitorPersonaje->GetCharacterMovement()->JumpZVelocity = SaltoNuevo;
}
