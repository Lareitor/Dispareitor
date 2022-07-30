#include "BuffComponente.h"
#include "Dispareitor/Personaje/DispareitorPersonaje.h"

UBuffComponente::UBuffComponente() {
	PrimaryComponentTick.bCanEverTick = true;
}

void UBuffComponente::BeginPlay() {
	Super::BeginPlay();
	
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


