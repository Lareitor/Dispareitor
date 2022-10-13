#include "CompensacionLagComponente.h"
#include "Dispareitor/Personaje/DispareitorPersonaje.h"
#include "Dispareitor/Arma/Arma.h"
#include "Components/BoxComponent.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Dispareitor/Dispareitor.h"


UCompensacionLagComponente::UCompensacionLagComponente() {
	PrimaryComponentTick.bCanEverTick = true;
}

void UCompensacionLagComponente::BeginPlay() {
	Super::BeginPlay();
}

void UCompensacionLagComponente::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	GuardarCajasImpactoFrame();
}

void UCompensacionLagComponente::GuardarCajasImpactoFrame() {
	if(DispareitorPersonaje == nullptr || !DispareitorPersonaje->HasAuthority()) {
		return;
	}

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
		// MostrarCajasImpactoFrame(CajasImpactoFrame, FColor::Orange);
	}
}

void UCompensacionLagComponente::GuardarCajasImpactoFrame(FCajasImpactoFrame& CajasImpactoFrame) {
	DispareitorPersonaje = DispareitorPersonaje != nullptr ? DispareitorPersonaje : Cast<ADispareitorPersonaje>(GetOwner());
	if(DispareitorPersonaje) {
		CajasImpactoFrame.Tiempo = GetWorld()->GetTimeSeconds();
		CajasImpactoFrame.DispareitorPersonaje = DispareitorPersonaje;
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

FResultadoRebobinarLadoServidor UCompensacionLagComponente::RebobinarLadoServidorHitscan(class ADispareitorPersonaje* DispareitorPersonajeImpactado, const FVector_NetQuantize& InicioRayo, const FVector_NetQuantize& ImpactoRayo, float TiempoImpacto) {
	FCajasImpactoFrame CajasImpactoFrameAComprobar = ObtenerCajasImpactoFrameAComprobar(DispareitorPersonajeImpactado, TiempoImpacto);
	return ConfirmarImpactoHitscan(CajasImpactoFrameAComprobar, DispareitorPersonajeImpactado, InicioRayo, ImpactoRayo);
}

FResultadoRebobinarLadoServidor UCompensacionLagComponente::RebobinarLadoServidorProyectil(class ADispareitorPersonaje* DispareitorPersonajeImpactado, const FVector_NetQuantize& InicioRayo, const FVector_NetQuantize100& VelocidadInicial, float TiempoImpacto) {
	FCajasImpactoFrame CajasImpactoFrameAComprobar = ObtenerCajasImpactoFrameAComprobar(DispareitorPersonajeImpactado, TiempoImpacto);
	return ConfirmarImpactoProyectil(CajasImpactoFrameAComprobar, DispareitorPersonajeImpactado, InicioRayo, VelocidadInicial, TiempoImpacto);
}

FResultadoRebobinarLadoServidorEscopeta UCompensacionLagComponente::RebobinarLadoServidorEscopeta(const TArray<ADispareitorPersonaje*>& DispareitorPersonajesImpactados, const FVector_NetQuantize& InicioRayo, const TArray<FVector_NetQuantize>& ImpactosRayos, float TiempoImpacto) {
	TArray<FCajasImpactoFrame> ArrayCajasImpactoFrameAComprobar;	
	for(ADispareitorPersonaje* DispareitorPersonajeImpactado : DispareitorPersonajesImpactados) {
		ArrayCajasImpactoFrameAComprobar.Add(ObtenerCajasImpactoFrameAComprobar(DispareitorPersonajeImpactado, TiempoImpacto));
	}

	return ConfirmarImpactoEscopeta(ArrayCajasImpactoFrameAComprobar, InicioRayo, ImpactosRayos);
}


FCajasImpactoFrame UCompensacionLagComponente::ObtenerCajasImpactoFrameAComprobar(ADispareitorPersonaje* DispareitorPersonajeImpactado, float TiempoImpacto) {
	if(DispareitorPersonajeImpactado == nullptr || DispareitorPersonajeImpactado->ObtenerCompensacionLagComponente() == nullptr || 
			DispareitorPersonajeImpactado->ObtenerCompensacionLagComponente()->CajasImpactoFrames.GetHead() == nullptr || 
			DispareitorPersonajeImpactado->ObtenerCompensacionLagComponente()->CajasImpactoFrames.GetTail() == nullptr ) {
		return FCajasImpactoFrame();
	}

	FCajasImpactoFrame CajasImpactoFrameAComprobar;
	bool bDeboInterpolar = true;

	const TDoubleLinkedList<FCajasImpactoFrame>& CajasImpactoFramesDelPersonajeImpactado = DispareitorPersonajeImpactado->ObtenerCompensacionLagComponente()->CajasImpactoFrames;
	const float TiempoMasAntiguo = CajasImpactoFramesDelPersonajeImpactado.GetTail()->GetValue().Tiempo;
	const float TiempoMasNuevo = CajasImpactoFramesDelPersonajeImpactado.GetHead()->GetValue().Tiempo;
	if(TiempoMasAntiguo > TiempoImpacto) { // Demasiado atrás en el tiempo, demasiado lageado para hacer el Server Side Rewind
		return FCajasImpactoFrame();
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
		CajasImpactoFrameAComprobar = InterpolacionEntreFrames(CajasImpactoFrameMasViejo->GetValue(), CajasImpactoFrameMasJoven->GetValue(), TiempoImpacto);	
	}

	CajasImpactoFrameAComprobar.DispareitorPersonaje = DispareitorPersonajeImpactado;
	return CajasImpactoFrameAComprobar;
}

FCajasImpactoFrame UCompensacionLagComponente::InterpolacionEntreFrames(const FCajasImpactoFrame& CajasImpactoFrameMasJoven, const FCajasImpactoFrame& CajasImpactoFrameMasViejo, float TiempoImpacto) {
	const float Distancia = CajasImpactoFrameMasJoven.Tiempo - CajasImpactoFrameMasViejo.Tiempo;
	const float FraccionDeInterporlacion = FMath::Clamp((TiempoImpacto - CajasImpactoFrameMasViejo.Tiempo) / Distancia, 0.f, 1.f);

	FCajasImpactoFrame CajasImpactoFrameInterpolado;
	CajasImpactoFrameInterpolado.Tiempo = TiempoImpacto;

	for(auto& CajasImpacto : CajasImpactoFrameMasJoven.CajasImpacto) {
		const FName& NombreCajaImpacto = CajasImpacto.Key;
		const FCajaImpacto& CajaImpactoMasVieja = CajasImpactoFrameMasViejo.CajasImpacto[NombreCajaImpacto];
		const FCajaImpacto& CajaImpactoMasJoven = CajasImpactoFrameMasJoven.CajasImpacto[NombreCajaImpacto];

		FCajaImpacto CajaImpactoInterpolada;
		CajaImpactoInterpolada.Posicion = FMath::VInterpTo(CajaImpactoMasVieja.Posicion, CajaImpactoMasJoven.Posicion, 1.f, FraccionDeInterporlacion);
		CajaImpactoInterpolada.Rotacion = FMath::RInterpTo(CajaImpactoMasVieja.Rotacion, CajaImpactoMasJoven.Rotacion, 1.f, FraccionDeInterporlacion);
		CajaImpactoInterpolada.CajaExtension = CajaImpactoMasJoven.CajaExtension;

		CajasImpactoFrameInterpolado.CajasImpacto.Add(NombreCajaImpacto, CajaImpactoInterpolada);
	}

	return CajasImpactoFrameInterpolado;
}

FResultadoRebobinarLadoServidor UCompensacionLagComponente::ConfirmarImpactoHitscan(const FCajasImpactoFrame& CajasImpactoFrame, ADispareitorPersonaje* DispareitorPersonajeImpactado, const FVector_NetQuantize& InicioRayo, const FVector_NetQuantize& ImpactoRayo) {
	UWorld* Mundo = GetWorld();
	
	if(!DispareitorPersonajeImpactado || !Mundo) {
		return FResultadoRebobinarLadoServidor();
	}

	FCajasImpactoFrame CajasImpactoFrameActual;
	CachearCajasImpactoFrame(DispareitorPersonajeImpactado, CajasImpactoFrameActual);
	MoverCajasImpactoFrame(DispareitorPersonajeImpactado, CajasImpactoFrame);
	ModificarColisionMallaPersonaje(DispareitorPersonajeImpactado, ECollisionEnabled::NoCollision);

	UBoxComponent* CajaCabeza = DispareitorPersonajeImpactado->CajasColision[FName("head")];
	CajaCabeza->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CajaCabeza->SetCollisionResponseToChannel(ECC_CajaColision, ECollisionResponse::ECR_Block);

	FHitResult ConfirmacionImpacto;
	const FVector FinRayo = InicioRayo + (ImpactoRayo - InicioRayo) * 1.25; // Para extender el rayo a traves del objeto
	
	
	Mundo->LineTraceSingleByChannel(ConfirmacionImpacto, InicioRayo, FinRayo, ECC_CajaColision);
	if(ConfirmacionImpacto.bBlockingHit) { // Golpeamos la cabeza, no necesitamos comprobar nada mas
		if(ConfirmacionImpacto.Component.IsValid()) {
			UBoxComponent* CajaImpacto = Cast<UBoxComponent>(ConfirmacionImpacto.Component);
			/*if(CajaImpacto) {
				DrawDebugBox(Mundo, CajaImpacto->GetComponentLocation(), CajaImpacto->GetScaledBoxExtent(), FQuat(CajaImpacto->GetComponentRotation()), FColor::Red, false, 8.f);
			}*/
		}

		RestaurarCajasImpactoFrame(DispareitorPersonajeImpactado, CajasImpactoFrameActual);
		ModificarColisionMallaPersonaje(DispareitorPersonajeImpactado, ECollisionEnabled::QueryAndPhysics);
		return FResultadoRebobinarLadoServidor{true, true};
	} else { // Comprobar si impactamos para el resto de cajas 
		for(auto& CajaColision : DispareitorPersonajeImpactado->CajasColision) {
			if(CajaColision.Value != nullptr) {
				CajaColision.Value->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
				CajaColision.Value->SetCollisionResponseToChannel(ECC_CajaColision, ECollisionResponse::ECR_Block);
			}
		}
		Mundo->LineTraceSingleByChannel(ConfirmacionImpacto, InicioRayo, FinRayo, ECC_CajaColision);
		if(ConfirmacionImpacto.bBlockingHit) {
			if(ConfirmacionImpacto.Component.IsValid()) {
				UBoxComponent* CajaImpacto = Cast<UBoxComponent>(ConfirmacionImpacto.Component);
				/*if(CajaImpacto) {
					DrawDebugBox(Mundo, CajaImpacto->GetComponentLocation(), CajaImpacto->GetScaledBoxExtent(), FQuat(CajaImpacto->GetComponentRotation()), FColor::Blue, false, 8.f);
				}*/
			}

			RestaurarCajasImpactoFrame(DispareitorPersonajeImpactado, CajasImpactoFrameActual);
			ModificarColisionMallaPersonaje(DispareitorPersonajeImpactado, ECollisionEnabled::QueryAndPhysics);
			return FResultadoRebobinarLadoServidor{true, false};
		} 
	}
	

	RestaurarCajasImpactoFrame(DispareitorPersonajeImpactado, CajasImpactoFrameActual);
	ModificarColisionMallaPersonaje(DispareitorPersonajeImpactado, ECollisionEnabled::QueryAndPhysics);
	return FResultadoRebobinarLadoServidor{false, false};
}

FResultadoRebobinarLadoServidor UCompensacionLagComponente::ConfirmarImpactoProyectil(const FCajasImpactoFrame& CajasImpactoFrame, ADispareitorPersonaje* DispareitorPersonajeImpactado, const FVector_NetQuantize& InicioRayo, const FVector_NetQuantize100& VelocidadInicial, float TiempoImpacto) {
	UWorld* Mundo = GetWorld();
	
	if(!DispareitorPersonajeImpactado || !Mundo) {
		return FResultadoRebobinarLadoServidor();
	}

	FCajasImpactoFrame CajasImpactoFrameActual;
	CachearCajasImpactoFrame(DispareitorPersonajeImpactado, CajasImpactoFrameActual);
	MoverCajasImpactoFrame(DispareitorPersonajeImpactado, CajasImpactoFrame);
	ModificarColisionMallaPersonaje(DispareitorPersonajeImpactado, ECollisionEnabled::NoCollision);

	UBoxComponent* CajaCabeza = DispareitorPersonajeImpactado->CajasColision[FName("head")];
	CajaCabeza->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CajaCabeza->SetCollisionResponseToChannel(ECC_CajaColision, ECollisionResponse::ECR_Block);

	FPredictProjectilePathParams ParametrosPredecirTrayectoriaProyectil;
	ParametrosPredecirTrayectoriaProyectil.bTraceWithChannel = true; // Calcular trayectoria con un canal especifico 
    ParametrosPredecirTrayectoriaProyectil.bTraceWithCollision = true; // Nos permite generar eventos de colision
    ParametrosPredecirTrayectoriaProyectil.LaunchVelocity = VelocidadInicial;
    ParametrosPredecirTrayectoriaProyectil.MaxSimTime = TiempoAlmacenamientoMaximo; // Tiempo máximo que el proyectil será simulado volando
    ParametrosPredecirTrayectoriaProyectil.ProjectileRadius = 5.f;
    ParametrosPredecirTrayectoriaProyectil.SimFrequency = 15.f; // Como de real es la simulación, cuanto mayor sea el nº más exacta será
    ParametrosPredecirTrayectoriaProyectil.StartLocation = InicioRayo;
    ParametrosPredecirTrayectoriaProyectil.TraceChannel = ECC_CajaColision;
    ParametrosPredecirTrayectoriaProyectil.ActorsToIgnore.Add(GetOwner());
	//ParametrosPredecirTrayectoriaProyectil.DrawDebugTime = 5.f; // Dibujar la trayectoria durante 5 sg.
    //ParametrosPredecirTrayectoriaProyectil.DrawDebugType = EDrawDebugTrace::ForDuration; // Para la duracion de 5 sg.
	FPredictProjectilePathResult ResultadoPredecirTrayectoriaProyectil;
	UGameplayStatics::PredictProjectilePath(this, ParametrosPredecirTrayectoriaProyectil, ResultadoPredecirTrayectoriaProyectil);

	
	if(ResultadoPredecirTrayectoriaProyectil.HitResult.bBlockingHit) { // Golpeamos la cabeza, no necesitamos comprobar nada mas
		if(ResultadoPredecirTrayectoriaProyectil.HitResult.Component.IsValid()) {
			UBoxComponent* CajaImpacto = Cast<UBoxComponent>(ResultadoPredecirTrayectoriaProyectil.HitResult.Component);
			/*if(CajaImpacto) {
				DrawDebugBox(Mundo, CajaImpacto->GetComponentLocation(), CajaImpacto->GetScaledBoxExtent(), FQuat(CajaImpacto->GetComponentRotation()), FColor::Red, false, 8.f);
			}*/
		}
		
		RestaurarCajasImpactoFrame(DispareitorPersonajeImpactado, CajasImpactoFrameActual);
		ModificarColisionMallaPersonaje(DispareitorPersonajeImpactado, ECollisionEnabled::QueryAndPhysics);
		return FResultadoRebobinarLadoServidor{true, true};
	} else { // Comprobar si impactamos para el resto de cajas 
		for(auto& CajaColision : DispareitorPersonajeImpactado->CajasColision) {
			if(CajaColision.Value != nullptr) {
				CajaColision.Value->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
				CajaColision.Value->SetCollisionResponseToChannel(ECC_CajaColision, ECollisionResponse::ECR_Block);
			}
		}

		UGameplayStatics::PredictProjectilePath(this, ParametrosPredecirTrayectoriaProyectil, ResultadoPredecirTrayectoriaProyectil);
		if(ResultadoPredecirTrayectoriaProyectil.HitResult.bBlockingHit) {
			if(ResultadoPredecirTrayectoriaProyectil.HitResult.Component.IsValid()) {
				UBoxComponent* CajaImpacto = Cast<UBoxComponent>(ResultadoPredecirTrayectoriaProyectil.HitResult.Component);
				/*if(CajaImpacto) {
					DrawDebugBox(Mundo, CajaImpacto->GetComponentLocation(), CajaImpacto->GetScaledBoxExtent(), FQuat(CajaImpacto->GetComponentRotation()), FColor::Blue, false, 8.f);
				}*/
			}

			RestaurarCajasImpactoFrame(DispareitorPersonajeImpactado, CajasImpactoFrameActual);
			ModificarColisionMallaPersonaje(DispareitorPersonajeImpactado, ECollisionEnabled::QueryAndPhysics);
			return FResultadoRebobinarLadoServidor{true, false};
		}
	}

	RestaurarCajasImpactoFrame(DispareitorPersonajeImpactado, CajasImpactoFrameActual);
	ModificarColisionMallaPersonaje(DispareitorPersonajeImpactado, ECollisionEnabled::QueryAndPhysics);
	return FResultadoRebobinarLadoServidor{false, false};
}


FResultadoRebobinarLadoServidorEscopeta UCompensacionLagComponente::ConfirmarImpactoEscopeta(const TArray<FCajasImpactoFrame>& ArrayCajasImpactoFrame, const FVector_NetQuantize& InicioRayo, const TArray<FVector_NetQuantize>& ImpactosRayos) {
	UWorld* Mundo = GetWorld();
	if(!Mundo) {
		return FResultadoRebobinarLadoServidorEscopeta();
	}
	
	for(auto& CajasImpactoFrame : ArrayCajasImpactoFrame) {
		if(!CajasImpactoFrame.DispareitorPersonaje) {
			return FResultadoRebobinarLadoServidorEscopeta();
		}
	}
	
	FResultadoRebobinarLadoServidorEscopeta ResultadoRebobinarLadoServidorEscopeta;
	TArray<FCajasImpactoFrame> ArrayCajasImpactoFrameActuales;

	for(auto& CajasImpactoFrame : ArrayCajasImpactoFrame) {
		FCajasImpactoFrame CajasImpactoFrameActual;

		CajasImpactoFrameActual.DispareitorPersonaje = CajasImpactoFrame.DispareitorPersonaje;
		CachearCajasImpactoFrame(CajasImpactoFrame.DispareitorPersonaje, CajasImpactoFrameActual);
		MoverCajasImpactoFrame(CajasImpactoFrame.DispareitorPersonaje, CajasImpactoFrame);
		ModificarColisionMallaPersonaje(CajasImpactoFrame.DispareitorPersonaje, ECollisionEnabled::NoCollision);
		ArrayCajasImpactoFrameActuales.Add(CajasImpactoFrameActual);
	}

	
	// Permitir colisiones solo para las cajas de cabeza
	for(auto& CajasImpactoFrame : ArrayCajasImpactoFrame) {
		UBoxComponent* CajaCabeza = CajasImpactoFrame.DispareitorPersonaje->CajasColision[FName("head")];
		CajaCabeza->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		CajaCabeza->SetCollisionResponseToChannel(ECC_CajaColision, ECollisionResponse::ECR_Block);
	}

	// Comprobar disparos en la cabeza
	for(auto& ImpactoRayo : ImpactosRayos) {	
		FHitResult ConfirmacionImpacto;
		const FVector FinRayo = InicioRayo + (ImpactoRayo - InicioRayo) * 1.25; // Para extender el rayo a traves del objeto
		
		Mundo->LineTraceSingleByChannel(ConfirmacionImpacto, InicioRayo, FinRayo, ECC_CajaColision);
		ADispareitorPersonaje* DispareitorPersonajeImpactado = Cast<ADispareitorPersonaje>(ConfirmacionImpacto.GetActor());
		if(DispareitorPersonajeImpactado) {
			if(ResultadoRebobinarLadoServidorEscopeta.TirosALaCabeza.Contains(DispareitorPersonajeImpactado)) {
				ResultadoRebobinarLadoServidorEscopeta.TirosALaCabeza[DispareitorPersonajeImpactado]++;
			} else {
				ResultadoRebobinarLadoServidorEscopeta.TirosALaCabeza.Emplace(DispareitorPersonajeImpactado, 1);
			}
		}
	}

	// Permitir colisiones para todas las cajas excepto para las de cabeza
	for(auto& CajasImpactoFrame : ArrayCajasImpactoFrame) {
		for(auto& CajaColision : CajasImpactoFrame.DispareitorPersonaje->CajasColision) {
			if(CajaColision.Value != nullptr) {
				CajaColision.Value->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
				CajaColision.Value->SetCollisionResponseToChannel(ECC_CajaColision, ECollisionResponse::ECR_Block);
			}
		}
		UBoxComponent* CajaCabeza = CajasImpactoFrame.DispareitorPersonaje->CajasColision[FName("head")];
		CajaCabeza->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	// Comprobar disparos en el cuerpo
	for(auto& ImpactoRayo : ImpactosRayos) {	
		FHitResult ConfirmacionImpacto;
		const FVector FinRayo = InicioRayo + (ImpactoRayo - InicioRayo) * 1.25; // Para extender el rayo a traves del objeto
		
		Mundo->LineTraceSingleByChannel(ConfirmacionImpacto, InicioRayo, FinRayo, ECC_CajaColision);
		ADispareitorPersonaje* DispareitorPersonajeImpactado = Cast<ADispareitorPersonaje>(ConfirmacionImpacto.GetActor());
		if(DispareitorPersonajeImpactado) {
			if(ResultadoRebobinarLadoServidorEscopeta.TirosAlCuerpo.Contains(DispareitorPersonajeImpactado)) {
				ResultadoRebobinarLadoServidorEscopeta.TirosAlCuerpo[DispareitorPersonajeImpactado]++;
			} else {
				ResultadoRebobinarLadoServidorEscopeta.TirosAlCuerpo.Emplace(DispareitorPersonajeImpactado, 1);
			}
		}
	}

	for(auto& CajasImpactoFrameActuales : ArrayCajasImpactoFrameActuales) {
		RestaurarCajasImpactoFrame(CajasImpactoFrameActuales.DispareitorPersonaje, CajasImpactoFrameActuales);
		ModificarColisionMallaPersonaje(CajasImpactoFrameActuales.DispareitorPersonaje, ECollisionEnabled::QueryAndPhysics);
	}

	return ResultadoRebobinarLadoServidorEscopeta;
}

void UCompensacionLagComponente::CachearCajasImpactoFrame(ADispareitorPersonaje* DispareitorPersonajeImpactado, FCajasImpactoFrame& CajasImpactoFrameSalida) {
	if(DispareitorPersonajeImpactado == nullptr) {
		return;
	}
	
	for(auto& CajaColision : DispareitorPersonajeImpactado->CajasColision) {
		if(CajaColision.Value != nullptr) {
			FCajaImpacto CajaImpacto;
			CajaImpacto.Posicion = CajaColision.Value->GetComponentLocation();
			CajaImpacto.Rotacion = CajaColision.Value->GetComponentRotation();
			CajaImpacto.CajaExtension = CajaColision.Value->GetScaledBoxExtent();
			CajasImpactoFrameSalida.CajasImpacto.Add(CajaColision.Key, CajaImpacto);
		}
	}
}

void UCompensacionLagComponente::MoverCajasImpactoFrame(ADispareitorPersonaje* DispareitorPersonajeImpactado,const FCajasImpactoFrame& CajasImpactoFrame) {
	if(DispareitorPersonajeImpactado == nullptr) {
		return;
	}

	for(auto& CajaColision : DispareitorPersonajeImpactado->CajasColision) {
		if(CajaColision.Value != nullptr) {
			CajaColision.Value->SetWorldLocation(CajasImpactoFrame.CajasImpacto[CajaColision.Key].Posicion);
			CajaColision.Value->SetWorldRotation(CajasImpactoFrame.CajasImpacto[CajaColision.Key].Rotacion);
			CajaColision.Value->SetBoxExtent(CajasImpactoFrame.CajasImpacto[CajaColision.Key].CajaExtension);
			
		}
	}
}

void UCompensacionLagComponente::RestaurarCajasImpactoFrame(ADispareitorPersonaje* DispareitorPersonajeImpactado,const FCajasImpactoFrame& CajasImpactoFrame) {
	if(DispareitorPersonajeImpactado == nullptr) {
		return;
	}

	for(auto& CajaColision : DispareitorPersonajeImpactado->CajasColision) {
		if(CajaColision.Value != nullptr) {
			CajaColision.Value->SetWorldLocation(CajasImpactoFrame.CajasImpacto[CajaColision.Key].Posicion);
			CajaColision.Value->SetWorldRotation(CajasImpactoFrame.CajasImpacto[CajaColision.Key].Rotacion);
			CajaColision.Value->SetBoxExtent(CajasImpactoFrame.CajasImpacto[CajaColision.Key].CajaExtension);
			CajaColision.Value->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
	}
}

void UCompensacionLagComponente::ModificarColisionMallaPersonaje(ADispareitorPersonaje* DispareitorPersonajeImpactado, ECollisionEnabled::Type TipoColisionPermitida) {
	if(DispareitorPersonajeImpactado && DispareitorPersonajeImpactado->GetMesh()) {
		DispareitorPersonajeImpactado->GetMesh()->SetCollisionEnabled(TipoColisionPermitida);
	}
}


void UCompensacionLagComponente::PeticionImpactoHitscan_EnServidor_Implementation(ADispareitorPersonaje* DispareitorPersonajeImpactado, const FVector_NetQuantize& InicioRayo, const FVector_NetQuantize& ImpactoRayo, float TiempoImpacto, class AArma* ArmaCausanteDanio) {
	FResultadoRebobinarLadoServidor ResultadoRebobinarLadoServidor = RebobinarLadoServidorHitscan(DispareitorPersonajeImpactado, InicioRayo, ImpactoRayo, TiempoImpacto);	

	if(DispareitorPersonaje && DispareitorPersonajeImpactado && ArmaCausanteDanio && ResultadoRebobinarLadoServidor.bImpactoConfirmado) {
		UGameplayStatics::ApplyDamage(DispareitorPersonajeImpactado, ArmaCausanteDanio->ObtenerDanio(), DispareitorPersonaje->Controller, ArmaCausanteDanio, UDamageType::StaticClass());
	}
}

void UCompensacionLagComponente::PeticionImpactoProyectil_EnServidor_Implementation(ADispareitorPersonaje* DispareitorPersonajeImpactado, const FVector_NetQuantize& InicioRayo, const FVector_NetQuantize100& VelocidadInicial, float TiempoImpacto) {
	FResultadoRebobinarLadoServidor ResultadoRebobinarLadoServidor = RebobinarLadoServidorProyectil(DispareitorPersonajeImpactado, InicioRayo, VelocidadInicial, TiempoImpacto);	

	if(DispareitorPersonaje && DispareitorPersonajeImpactado && ResultadoRebobinarLadoServidor.bImpactoConfirmado) {
		UGameplayStatics::ApplyDamage(DispareitorPersonajeImpactado, DispareitorPersonaje->ObtenerArmaEquipada()->ObtenerDanio(), DispareitorPersonaje->Controller, DispareitorPersonaje->ObtenerArmaEquipada(), UDamageType::StaticClass());
	}
}


void UCompensacionLagComponente::PeticionImpactoEscopeta_EnServidor_Implementation(const TArray<ADispareitorPersonaje*>& DispareitorPersonajesImpactados, const FVector_NetQuantize& InicioRayo, const TArray<FVector_NetQuantize>& ImpactosRayos, float TiempoImpacto) {
	FResultadoRebobinarLadoServidorEscopeta ResultadoRebobinarLadoServidorEscopeta = RebobinarLadoServidorEscopeta(DispareitorPersonajesImpactados, InicioRayo, ImpactosRayos, TiempoImpacto);

	for(auto& DispareitorPersonajeImpactado : DispareitorPersonajesImpactados) {
		if(DispareitorPersonajeImpactado && DispareitorPersonajeImpactado->ObtenerArmaEquipada() && DispareitorPersonaje) {
			float DanioTotal = 0.f;
			if(ResultadoRebobinarLadoServidorEscopeta.TirosALaCabeza.Contains(DispareitorPersonajeImpactado)) {
				float DanioTiroALaCabeza = ResultadoRebobinarLadoServidorEscopeta.TirosALaCabeza[DispareitorPersonajeImpactado] * DispareitorPersonajeImpactado->ObtenerArmaEquipada()->ObtenerDanio();
				DanioTotal += DanioTiroALaCabeza;
			}
			if(ResultadoRebobinarLadoServidorEscopeta.TirosAlCuerpo.Contains(DispareitorPersonajeImpactado)) {
				float DanioTiroAlCuerpo = ResultadoRebobinarLadoServidorEscopeta.TirosAlCuerpo[DispareitorPersonajeImpactado] * DispareitorPersonajeImpactado->ObtenerArmaEquipada()->ObtenerDanio();
				DanioTotal += DanioTiroAlCuerpo;
			}
			UGameplayStatics::ApplyDamage(DispareitorPersonajeImpactado, DanioTotal, DispareitorPersonaje->Controller, DispareitorPersonajeImpactado->ObtenerArmaEquipada(), UDamageType::StaticClass());

		}
	}
}
