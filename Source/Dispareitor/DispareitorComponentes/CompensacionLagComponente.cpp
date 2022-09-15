#include "CompensacionLagComponente.h"
#include "Dispareitor/Personaje/DispareitorPersonaje.h"
#include "Components/BoxComponent.h"
#include "DrawDebugHelpers.h"

UCompensacionLagComponente::UCompensacionLagComponente() {
	PrimaryComponentTick.bCanEverTick = true;

}

void UCompensacionLagComponente::BeginPlay() {
	Super::BeginPlay();

	FCajasImpactoFrame CajasImpactoFrame;
	GuardarCajasImpactoFrame(CajasImpactoFrame);
	MostrarCajasImpactoFrame(CajasImpactoFrame, FColor::Orange);
}

void UCompensacionLagComponente::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UCompensacionLagComponente::GuardarCajasImpactoFrame(FCajasImpactoFrame& CajasImpactoFrame) {
	DispareitorPersonaje = DispareitorPersonaje != nullptr ? DispareitorPersonaje : Cast<ADispareitorPersonaje>(GetOwner());
	if(DispareitorPersonaje) {
		CajasImpactoFrame.Tiempo = GetWorld()->GetTimeSeconds();
		for(auto& CajaColision : DispareitorPersonaje->CajasColision) {
			FCajaImpacto CajaImpacto;
			CajaImpacto.Posicion = CajaColision.Value->GetComponentLocation();
			CajaImpacto.Rotacion = CajaColision.Value->GetComponentRotation();
			CajaImpacto.CajaExtension = CajaColision.Value->GetScaledBoxExtent();
			CajasImpactoFrame.CajasImpacto.Add(CajaColision.Key, CajaImpacto);
		}
	}
}

void UCompensacionLagComponente::MostrarCajasImpactoFrame(const FCajasImpactoFrame& CajasImpactoFrame, const FColor Color) {
	for(auto& CajaImpacto : CajasImpactoFrame.CajasImpacto) {
		DrawDebugBox(GetWorld(), CajaImpacto.Value.Posicion, CajaImpacto.Value.CajaExtension, FQuat(CajaImpacto.Value.Rotacion), Color, true);
	}
}
