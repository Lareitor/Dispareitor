#include "CompensacionLagComponente.h"
#include "Dispareitor/Personaje/DispareitorPersonaje.h"
#include "Components/BoxComponent.h"
#include "DrawDebugHelpers.h"

UCompensacionLagComponente::UCompensacionLagComponente() {
	PrimaryComponentTick.bCanEverTick = true;
}

void UCompensacionLagComponente::BeginPlay() {
	Super::BeginPlay();
}

void UCompensacionLagComponente::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if(CajasImpactoFrames.Num() <= 1) {
		FCajasImpactoFrame CajasImpactoFrame;
		GuardarCajasImpactoFrame(CajasImpactoFrame);
		CajasImpactoFrames.AddHead(CajasImpactoFrame);
	} else {
		float TiempoAlmacenado = CajasImpactoFrames.GetHead()->GetValue().Tiempo - CajasImpactoFrames.GetTail()->GetValue().Tiempo; 
		while(TiempoAlmacenado > TiempoAlmacenamientoMaximo) {
			CajasImpactoFrames.RemoveNode(CajasImpactoFrames.GetTail());
			TiempoAlmacenado = CajasImpactoFrames.GetHead()->GetValue().Tiempo - CajasImpactoFrames.GetTail()->GetValue().Tiempo;
		}
		FCajasImpactoFrame CajasImpactoFrame;
		GuardarCajasImpactoFrame(CajasImpactoFrame);
		CajasImpactoFrames.AddHead(CajasImpactoFrame);
		MostrarCajasImpactoFrame(CajasImpactoFrame, FColor::Orange);
	}
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
		DrawDebugBox(GetWorld(), CajaImpacto.Value.Posicion, CajaImpacto.Value.CajaExtension, FQuat(CajaImpacto.Value.Rotacion), Color, false, 4.f);
	}
}

void UCompensacionLagComponente::RebobinarLadoServidor(class ADispareitorPersonaje* DispareitorPersonajeImpactado, const FVector_NetQuantize& InicioRayo, const FVector_NetQuantize& FinRayo, float TiempoImpacto) {
	if(DispareitorPersonajeImpactado == nullptr || DispareitorPersonajeImpactado->ObtenerCompensacionLagComponente() == nullptr || 
			DispareitorPersonajeImpactado->ObtenerCompensacionLagComponente()->CajasImpactoFrames.GetHead() == nullptr || 
			DispareitorPersonajeImpactado->ObtenerCompensacionLagComponente()->CajasImpactoFrames.GetTail()) {
		return;
	}

	FCajasImpactoFrame CajasImpactoFrameAComprobar;
	bool bDeboInterpolar = true;

	const TDoubleLinkedList<FCajasImpactoFrame>& CajasImpactoFramesDelPersonajeImpactado = DispareitorPersonajeImpactado->ObtenerCompensacionLagComponente()->CajasImpactoFrames;
	const float TiempoMasAntiguo = CajasImpactoFramesDelPersonajeImpactado.GetTail()->GetValue().Tiempo;
	const float TiempoMasNuevo = CajasImpactoFramesDelPersonajeImpactado.GetHead()->GetValue().Tiempo;
	if(TiempoMasAntiguo > TiempoImpacto) { // Demasiado atrás en el tiempo, demasiado lageado para hacer el Server Side Rewind
		return;
	}
	if(TiempoMasAntiguo == TiempoImpacto) { 
		CajasImpactoFrameAComprobar = CajasImpactoFramesDelPersonajeImpactado.GetTail()->GetValue();
		bDeboInterpolar = false;
	}
	if(TiempoMasNuevo <= TiempoImpacto) {
		CajasImpactoFrameAComprobar = CajasImpactoFramesDelPersonajeImpactado.GetHead()->GetValue();
		bDeboInterpolar = false;
	}

	TDoubleLinkedList<FCajasImpactoFrame>::TDoubleLinkedListNode* CajasImpactoFrameMasJoven = CajasImpactoFramesDelPersonajeImpactado.GetHead();
	TDoubleLinkedList<FCajasImpactoFrame>::TDoubleLinkedListNode* CajasImpactoFrameMasViejo = CajasImpactoFrameMasJoven;
	// Nos movemos hacia atras en el tiempo hasta que CajasImpactoFrameMasViejo < TiempoImpacto < CajasImpactoFrameMasJoven
	while(CajasImpactoFrameMasViejo->GetValue().Tiempo > TiempoImpacto) {
		if(CajasImpactoFrameMasViejo->GetNextNode() == nullptr) {
			break;
		}
		CajasImpactoFrameMasViejo = CajasImpactoFrameMasViejo->GetNextNode(); // GetNextNode obtiene el nodo anterior de la lista doble enlazada
		if(CajasImpactoFrameMasViejo->GetValue().Tiempo > TiempoImpacto) {
			CajasImpactoFrameMasJoven = CajasImpactoFrameMasViejo;
		}
	}
	if(CajasImpactoFrameMasViejo->GetValue().Tiempo == TiempoImpacto) {
		CajasImpactoFrameAComprobar = CajasImpactoFrameMasViejo->GetValue();
		bDeboInterpolar = false;
	}

	if(bDeboInterpolar) {

	}
}
