#include "CombateComponente.h"
#include "Dispareitor/Arma/Arma.h"
#include "Dispareitor/Personaje/DispareitorPersonaje.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "Dispareitor/ControladorJugador/DispareitorControladorJugador.h"
#include "Camera/CameraComponent.h"
#include "TimerManager.h"
#include "Sound/SoundCue.h"
#include "Dispareitor/Personaje/DispareitorInstanciaAnimacion.h"
#include "Dispareitor/Arma/Proyectil.h"

UCombateComponente::UCombateComponente() {
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(true);

	VelocidadCaminarBase = 600.f;
	VelocidadCaminarApuntando = 400.f;
}

void UCombateComponente::BeginPlay() {
	Super::BeginPlay();	

	if(DispareitorPersonaje) {
		DispareitorPersonaje->GetCharacterMovement()->MaxWalkSpeed = VelocidadCaminarBase;
		
		if(DispareitorPersonaje->ObtenerCamara()) {
			PorDefectoFOV = DispareitorPersonaje->ObtenerCamara()->FieldOfView;
			ActualFOV = PorDefectoFOV;
		}
	}

	if(DispareitorPersonaje->HasAuthority()) {
		MunicionPersonajeInicializar();
	}
}

void UCombateComponente::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UCombateComponente, ArmaEquipada);
	DOREPLIFETIME(UCombateComponente, bApuntando);
	DOREPLIFETIME_CONDITION(UCombateComponente, MunicionPersonaje, COND_OwnerOnly);
	DOREPLIFETIME(UCombateComponente, EstadoCombate);
}

void UCombateComponente::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if(DispareitorPersonaje && DispareitorPersonaje->IsLocallyControlled()) {
		FHitResult RayoResultado;
		CalcularRayoDesdeCruceta(RayoResultado);
		ObjetoAlcanzado = RayoResultado.ImpactPoint;

		ActualizarHUDCruceta(DeltaTime);
		InterpolarFOV(DeltaTime);
	}
}

// Llamado por Tick
void UCombateComponente::CalcularRayoDesdeCruceta(FHitResult& RayoResultado) {
	FVector2D PantallaTamano;
	if(GEngine && GEngine->GameViewport) {
		GEngine->GameViewport->GetViewportSize(PantallaTamano);
	}

	FVector2D CrucetaLocalizacion(PantallaTamano.X / 2.f, PantallaTamano.Y / 2.f);
	FVector CrucetaMundoPosicion;
	FVector CrucetaMundoDireccion;
	bool bPantallaAMundo = UGameplayStatics::DeprojectScreenToWorld(UGameplayStatics::GetPlayerController(this, 0), CrucetaLocalizacion, CrucetaMundoPosicion, CrucetaMundoDireccion);
	if(bPantallaAMundo) {
		// La cruceta se dibuja justo donde está la camara
		FVector Inicio = CrucetaMundoPosicion;

		// Para evitar que colisione con objetos entre la camara y el jugador, ponemos Inicio un pelin por delante del jugador
		if(DispareitorPersonaje) {
			float DistanciaCamaraAPersonaje = (DispareitorPersonaje->GetActorLocation() - Inicio).Size();
			Inicio += CrucetaMundoDireccion * (DistanciaCamaraAPersonaje + 100.f);
		}

		FVector Fin = Inicio + CrucetaMundoDireccion * RAYO_LONGITUD;
		GetWorld()->LineTraceSingleByChannel(RayoResultado, Inicio, Fin, ECollisionChannel::ECC_Visibility);
		// Si no hemos colisionado con nada, hacemos que el punto de impacto sea el final del rayo
		if(!RayoResultado.bBlockingHit) {
			RayoResultado.ImpactPoint = Fin;
		}

		if(RayoResultado.GetActor() && RayoResultado.GetActor()->Implements<UInteractuarConCrucetaInterfaz>()) {
			HUDCruceta.CrucetaColor = FLinearColor::Red;
		} else {
			HUDCruceta.CrucetaColor = FLinearColor::White;
		}
	}
}

// Llamado por Tick
void UCombateComponente::ActualizarHUDCruceta(float DeltaTime) {
	if(DispareitorPersonaje == nullptr || DispareitorPersonaje->Controller == nullptr) {
		return;
	}

	DispareitorControladorJugador = DispareitorControladorJugador == nullptr ? Cast<ADispareitorControladorJugador>(DispareitorPersonaje->Controller) : DispareitorControladorJugador; 
	if(DispareitorControladorJugador) {
		DispareitorHUD = DispareitorHUD == nullptr ? Cast<ADispareitorHUD>(DispareitorControladorJugador->GetHUD()) : DispareitorHUD;
		if(DispareitorHUD) {
			if(ArmaEquipada) {
				HUDCruceta.CrucetaCentro = ArmaEquipada->CrucetaCentro;
				HUDCruceta.CrucetaIzquierda = ArmaEquipada->CrucetaIzquierda;
				HUDCruceta.CrucetaDerecha = ArmaEquipada->CrucetaDerecha;
				HUDCruceta.CrucetaArriba = ArmaEquipada->CrucetaArriba;
				HUDCruceta.CrucetaAbajo = ArmaEquipada->CrucetaAbajo;
			} else {
				HUDCruceta.CrucetaCentro = nullptr;
				HUDCruceta.CrucetaIzquierda = nullptr;
				HUDCruceta.CrucetaDerecha = nullptr;
				HUDCruceta.CrucetaArriba = nullptr;
				HUDCruceta.CrucetaAbajo = nullptr;
			}

			// Mapear la velocidad del jugador al rango [0, 1] para controlar la apertura de la cruceta
			FVector2D VelocidadCaminarRango(0.f, DispareitorPersonaje->GetCharacterMovement()->MaxWalkSpeed);
			FVector2D VelocidadNormalizadaRango(0.f, 1.f);
			FVector Velocidad = DispareitorPersonaje->GetVelocity();
			Velocidad.Z = 0.f;
			CrucetaFactorVelocidad = FMath::GetMappedRangeValueClamped(VelocidadCaminarRango, VelocidadNormalizadaRango, Velocidad.Size());
			
			if(DispareitorPersonaje->GetCharacterMovement()->IsFalling()) {
				CrucetaFactorEnAire = FMath::FInterpTo(CrucetaFactorEnAire, 2.25f, DeltaTime, 2.25f);
			} else {
				CrucetaFactorEnAire = FMath::FInterpTo(CrucetaFactorEnAire, 0.f, DeltaTime, 30.f);
			}
			
			if(bApuntando) {
				CrucetaFactorApuntado =  FMath::FInterpTo(CrucetaFactorApuntado, 0.58f, DeltaTime, 30.f);
			} else {
				CrucetaFactorApuntado =  FMath::FInterpTo(CrucetaFactorApuntado, 0.f, DeltaTime, 30.f);

			}

			CrucetaFactorDisparo = FMath::FInterpTo(CrucetaFactorDisparo, 0.f, DeltaTime, 40.f);
			
			HUDCruceta.CrucetaApertura = 0.5f + CrucetaFactorVelocidad + CrucetaFactorEnAire - CrucetaFactorApuntado + CrucetaFactorDisparo;

			DispareitorHUD->ActualizarHUDCruceta(HUDCruceta);
		}
	}
}

// Llamado por Tick
void UCombateComponente::InterpolarFOV(float DeltaTime) {
	if(ArmaEquipada == nullptr) {
		return;
	}

	if(bApuntando) {
		ActualFOV = FMath::FInterpTo(ActualFOV, ArmaEquipada->ObtenerZoomFOV(), DeltaTime, ArmaEquipada->ObtenerVelocidadInterpolacion());
	} else {
		ActualFOV = FMath::FInterpTo(ActualFOV, PorDefectoFOV, DeltaTime, ZoomVelocidadInterpolacion);
	}
	if(DispareitorPersonaje && DispareitorPersonaje->ObtenerCamara()) {
		DispareitorPersonaje->ObtenerCamara()->SetFieldOfView(ActualFOV);
	}
}

// Llamado en BeginPlay
void UCombateComponente::MunicionPersonajeInicializar() {
	MunicionPersonajeMapa.Emplace(ETipoArma::ETA_RifleAsalto, MunicionPersonajeInicialRifleAsalto);
	MunicionPersonajeMapa.Emplace(ETipoArma::ETA_LanzaCohetes, MunicionPersonajeInicialLanzaCohetes);
	MunicionPersonajeMapa.Emplace(ETipoArma::ETA_Pistola, MunicionPersonajeInicialPistola);
	MunicionPersonajeMapa.Emplace(ETipoArma::ETA_Subfusil, MunicionPersonajeInicialSubfusil);
	MunicionPersonajeMapa.Emplace(ETipoArma::ETA_Escopeta, MunicionPersonajeInicialEscopeta);
	MunicionPersonajeMapa.Emplace(ETipoArma::ETA_Francotirador, MunicionPersonajeInicialFrancotirador);
	MunicionPersonajeMapa.Emplace(ETipoArma::ETA_LanzaGranadas, MunicionPersonajeInicialLanzaGranadas);
}

// Llamado por ADispareitorPersonaje::Equipar 
// Solo ejecutado en el servidor
void UCombateComponente::EquiparArma(class AArma* ArmaAEquipar) {
	if(DispareitorPersonaje == nullptr || ArmaAEquipar == nullptr || EstadoCombate != EEstadosCombate::EEC_Desocupado) {
		return;
	}

	ArmaEquipadaSoltar();
	ArmaEquipada = ArmaAEquipar;
	ArmaEquipada->ActualizarEstado(EEstado::EEA_Equipada); // Se propaga al cliente
	ManoDerechaUnirActor(ArmaEquipada);
	ArmaEquipada->SetOwner(DispareitorPersonaje);	
	ArmaEquipada->ActualizarHUDMunicion();
	MunicionPersonajeActualizar();
	EquiparSonidoEjecutar();
	ArmaVaciaRecargar();

	DispareitorPersonaje->GetCharacterMovement()->bOrientRotationToMovement = false;
	DispareitorPersonaje->bUseControllerRotationYaw = true;
}

void UCombateComponente::ArmaEquipadaSoltar() {
	if(ArmaEquipada) {
		ArmaEquipada->Soltar();
	}
}

void UCombateComponente::ManoDerechaUnirActor(AActor* Actor) {
	if(DispareitorPersonaje == nullptr || DispareitorPersonaje->GetMesh() == nullptr || Actor == nullptr) {
		return;	
	}  

	const USkeletalMeshSocket* ManoDerechaSocket = DispareitorPersonaje->GetMesh()->GetSocketByName(FName("ManoDerechaSocket"));
	if(ManoDerechaSocket) {
		ManoDerechaSocket->AttachActor(Actor, DispareitorPersonaje->GetMesh()); // Tambien se propaga a los clientes, pero no hay garantias de cual se propaga antes 
	}
}

void UCombateComponente::ManoIzquierdaUnirActor(AActor* Actor) {
	if(DispareitorPersonaje == nullptr || DispareitorPersonaje->GetMesh() == nullptr || Actor == nullptr) {
		return;	
	}  

	const USkeletalMeshSocket* ManoIzquierdaSocket = DispareitorPersonaje->GetMesh()->GetSocketByName(FName("ManoIzquierdaSocket"));
	if(ManoIzquierdaSocket) {
		ManoIzquierdaSocket->AttachActor(Actor, DispareitorPersonaje->GetMesh()); // Tambien se propaga a los clientes, pero no hay garantias de cual se propaga antes 
	}
}

void UCombateComponente::ArmaVaciaRecargar() {
	if(ArmaEquipada && ArmaEquipada->EstaSinMunicion()) {
		Recargar();
	}
}

void UCombateComponente::MunicionPersonajeActualizar() {
	if(ArmaEquipada == nullptr) {
		return;
	}

	if(MunicionPersonajeMapa.Contains(ArmaEquipada->TipoArmaObtener())) {
		MunicionPersonaje = MunicionPersonajeMapa[ArmaEquipada->TipoArmaObtener()];
	}

	DispareitorControladorJugador = DispareitorControladorJugador != nullptr ? DispareitorControladorJugador : Cast<ADispareitorControladorJugador>(DispareitorPersonaje->Controller);
	if(DispareitorControladorJugador) {
		DispareitorControladorJugador->HUDPersonajeMunicionActualizar(MunicionPersonaje);
	}
}


void UCombateComponente::AlReplicarArmaEquipada() {
	if(ArmaEquipada && DispareitorPersonaje) {
		// Para garantizar que se ejecutan en orden las ejecutamos en los clientes en el orden correcto
		ArmaEquipada->ActualizarEstado(EEstado::EEA_Equipada); 
		ManoDerechaUnirActor(ArmaEquipada);
		DispareitorPersonaje->GetCharacterMovement()->bOrientRotationToMovement = false;
		DispareitorPersonaje->bUseControllerRotationYaw = true;
		EquiparSonidoEjecutar();
	}
}

void UCombateComponente::EquiparSonidoEjecutar() {
	if(DispareitorPersonaje && ArmaEquipada && ArmaEquipada->EquiparSonido) {
		UGameplayStatics::PlaySoundAtLocation(this, ArmaEquipada->EquiparSonido, DispareitorPersonaje->GetActorLocation());
	}
}

// Llamado por ADispareitorPersonaje::Recargar
// Puede ser llamado tanto en un cliente como en un servidor
void UCombateComponente::Recargar() {

	if(ArmaEquipada == nullptr || ArmaEquipada->EstaConMunicionLlena()) {
		return;
	}

	// Si lo ejecutamos desde el cliente podemos chequear si tiene municion para evitar llamadas innecesarias al servidor
	if(MunicionPersonaje > 0 && EstadoCombate == EEstadosCombate::EEC_Desocupado) {
		RecargarServidor();
	}
}

void UCombateComponente::RecargarServidor_Implementation() {
	EstadoCombate = EEstadosCombate::EEC_Recargando;
	RecargarManejador();
}

// Llamado por RecargarServidor_Implementation (para el servidor) y EstadoCombateAlReplicar (para los clientes)
void UCombateComponente::RecargarManejador() {
	if(DispareitorPersonaje) {
		DispareitorPersonaje->EjecutarMontajeRecargar();
	}
}

// Llamado por BP_DispareitorInstanciaAnimacion al ejecutar una notificacion en Montaje_Recargar
void UCombateComponente::RecargarFinalizado() {
	if(DispareitorPersonaje == nullptr) {
		return;
	}

	if(DispareitorPersonaje->HasAuthority()) {
		EstadoCombate = EEstadosCombate::EEC_Desocupado;
		MunicionActualizarValores();
	}

	if(bDispararPresionado) {
		Disparar();
	}
}

// Llamado por BP_DispareitorInstanciaAnimacion al ejecutar una notificacion en Montaje_Recargar al insertar un cartucho en la escopeta
void UCombateComponente::RecargarCartuchoEscopeta() {
	if(DispareitorPersonaje && DispareitorPersonaje->HasAuthority()) {
		MunicionEscopetaActualizarValores();
	}
}

void UCombateComponente::MunicionActualizarValores() {
	if(DispareitorPersonaje == nullptr || ArmaEquipada == nullptr || !MunicionPersonajeMapa.Contains(ArmaEquipada->TipoArmaObtener())) {
		return;
	}

	int32 RecargarCantidadValor = RecargarCantidad();
	MunicionPersonajeMapa[ArmaEquipada->TipoArmaObtener()] -= RecargarCantidadValor;
	MunicionPersonaje = MunicionPersonajeMapa[ArmaEquipada->TipoArmaObtener()];
	ArmaEquipada->MunicionAniadir(RecargarCantidadValor);

	DispareitorControladorJugador = DispareitorControladorJugador != nullptr ? DispareitorControladorJugador : Cast<ADispareitorControladorJugador>(DispareitorPersonaje->Controller);
	if(DispareitorControladorJugador) {
		DispareitorControladorJugador->HUDPersonajeMunicionActualizar(MunicionPersonaje);
	}
}

void UCombateComponente::MunicionEscopetaActualizarValores() {
	if(DispareitorPersonaje == nullptr || ArmaEquipada == nullptr || !MunicionPersonajeMapa.Contains(ArmaEquipada->TipoArmaObtener())) {
		return;
	}

	MunicionPersonajeMapa[ArmaEquipada->TipoArmaObtener()] -= 1;
	MunicionPersonaje = MunicionPersonajeMapa[ArmaEquipada->TipoArmaObtener()];
	DispareitorControladorJugador = DispareitorControladorJugador != nullptr ? DispareitorControladorJugador : Cast<ADispareitorControladorJugador>(DispareitorPersonaje->Controller);
	if(DispareitorControladorJugador) {
		DispareitorControladorJugador->HUDPersonajeMunicionActualizar(MunicionPersonaje);
	}
	ArmaEquipada->MunicionAniadir(1);
	bPuedoDisparar = true;
	if(ArmaEquipada->EstaConMunicionLlena() || MunicionPersonaje == 0) {
		EscopetaFinAnimacionSaltar();
	}
}

void UCombateComponente::EscopetaFinAnimacionSaltar() {
	UAnimInstance* InstanciaAnimacion = DispareitorPersonaje->GetMesh()->GetAnimInstance();
	if(InstanciaAnimacion && DispareitorPersonaje->MontajeRecargarObtener()) {
		InstanciaAnimacion->Montage_JumpToSection("EscopetaFin");
	}
}

void UCombateComponente::MunicionPersonajeAlReplicar() {
	DispareitorControladorJugador = DispareitorControladorJugador != nullptr ? DispareitorControladorJugador : Cast<ADispareitorControladorJugador>(DispareitorPersonaje->Controller);
	if(DispareitorControladorJugador) {
		DispareitorControladorJugador->HUDPersonajeMunicionActualizar(MunicionPersonaje);
	}

	if(EstadoCombate == EEstadosCombate::EEC_Recargando && ArmaEquipada != nullptr && ArmaEquipada->TipoArmaObtener() == ETipoArma::ETA_Escopeta && MunicionPersonaje == 0) {
		EscopetaFinAnimacionSaltar();
	}
}


int32 UCombateComponente::RecargarCantidad() {
	if(ArmaEquipada == nullptr || !MunicionPersonajeMapa.Contains(ArmaEquipada->TipoArmaObtener())) {
		return 0;
	}

	int32 CargadorEspacio = ArmaEquipada->CargadorCapacidadObtener() - ArmaEquipada->MunicionObtener();
	int32 MunicionPersonajeTemporal = MunicionPersonajeMapa[ArmaEquipada->TipoArmaObtener()];
	int32 Minimo = FMath::Min(CargadorEspacio, MunicionPersonajeTemporal);
	return FMath::Clamp(CargadorEspacio, 0, Minimo);
}


void UCombateComponente::EstadoCombateAlReplicar() {
	switch(EstadoCombate) {
		case EEstadosCombate::EEC_Recargando:
			RecargarManejador();
			break;
		case EEstadosCombate::EEC_Desocupado:
			if(bDispararPresionado) {
				Disparar();
			}
			break;	
		case EEstadosCombate::EEC_LanzandoGranada:
			if(DispareitorPersonaje && !DispareitorPersonaje->IsLocallyControlled()) {
				DispareitorPersonaje->GranadaArrojarMontajeEjecutar();
				ManoIzquierdaUnirActor(ArmaEquipada);
				GranadaMostrar(true);
			}
			break;	
	}
}

// Llamado por DispareitorPersonaje cuando se pulsa o libera el boton de apuntar
void UCombateComponente::ActualizarApuntando(bool Apuntando) {
	if(DispareitorPersonaje == nullptr || ArmaEquipada == nullptr) {
		return;
	}

	// Aunque esta funcion es posible que la llamemos desde el cliente, por cuestiones cosmeticas podemos hacerlo ahora
	bApuntando = Apuntando; 
	// Si lo estamos ejecutando en un cliente se invocará esta función en el servidor, y si lo estamos ejecutando en el servidor se ejecutará en él mismo
	ServidorActualizarApuntando(bApuntando);

	if(DispareitorPersonaje) {
		DispareitorPersonaje->GetCharacterMovement()->MaxWalkSpeed = bApuntando ? VelocidadCaminarApuntando : VelocidadCaminarBase;

		if (DispareitorPersonaje->IsLocallyControlled() && ArmaEquipada->TipoArmaObtener() == ETipoArma::ETA_Francotirador) {
			DispareitorControladorJugador = DispareitorControladorJugador != nullptr ? DispareitorControladorJugador :  Cast<ADispareitorControladorJugador>(DispareitorPersonaje->Controller);
			if (DispareitorControladorJugador) {
				DispareitorControladorJugador->HUDFrancotiradorCrucetaActualizar(Apuntando);
				if (Apuntando && FrancotiradorCrucetaZoomIn) {
					UGameplayStatics::PlaySound2D(this, FrancotiradorCrucetaZoomIn);
				} else if(FrancotiradorCrucetaZoomOut) {
					UGameplayStatics::PlaySound2D(this, FrancotiradorCrucetaZoomOut);
				}
			}
		}
	}
}

void UCombateComponente::ServidorActualizarApuntando_Implementation(bool Apuntando) {
	bApuntando = Apuntando; 
	if(DispareitorPersonaje) {
		DispareitorPersonaje->GetCharacterMovement()->MaxWalkSpeed = bApuntando ? VelocidadCaminarApuntando : VelocidadCaminarBase;
	} 
}
 
// Llamado por DispareitorPersonaje cuando se pulsa o libera el boton de disparar
void UCombateComponente::DispararPresionado(bool bPresionado) {
	bDispararPresionado = bPresionado;
	if(bDispararPresionado && ArmaEquipada) {
		Disparar();
	}	
}

void UCombateComponente::Disparar() {
	if(PuedoDisparar()) {
		bPuedoDisparar = false;

		// Si estamos en el server se ejecutar en el server y si estamos en un cliente se ejectura en el server
		ServidorDisparar(ObjetoAlcanzado);

		if(ArmaEquipada) {
			CrucetaFactorDisparo = 0.75f;
		}

		EmpezarDisparoTemporizador();
	}  
}

// Esta funcion solo se ejecutará en el servidor
void UCombateComponente::ServidorDisparar_Implementation(const FVector_NetQuantize& Objetivo) {
	MulticastDisparar(Objetivo);
}

// Esta función se ejecutará en el servidor + clientes
void UCombateComponente::MulticastDisparar_Implementation(const FVector_NetQuantize& Objetivo) {
	if(DispareitorPersonaje && ArmaEquipada) {
		if(EstadoCombate == EEstadosCombate::EEC_Desocupado) {
			DispareitorPersonaje->EjecutarMontajeDispararArma(bApuntando);
			ArmaEquipada->Disparar(Objetivo);
		} else if(EstadoCombate == EEstadosCombate::EEC_Recargando && ArmaEquipada->TipoArmaObtener() == ETipoArma::ETA_Escopeta) {
			DispareitorPersonaje->EjecutarMontajeDispararArma(bApuntando);
			ArmaEquipada->Disparar(Objetivo);
			EstadoCombate = EEstadosCombate::EEC_Desocupado;
		}
	} 
}

void UCombateComponente::EmpezarDisparoTemporizador() {
	if(DispareitorPersonaje == nullptr || ArmaEquipada == nullptr) {
		return;
	}

	DispareitorPersonaje->GetWorldTimerManager().SetTimer(DisparoTemporizador, this, &UCombateComponente::TerminadoDisparoTemporizador, ArmaEquipada->DisparoRetardo);
}

void UCombateComponente::TerminadoDisparoTemporizador() {
	if(DispareitorPersonaje == nullptr || ArmaEquipada == nullptr) {
		return;
	}

	bPuedoDisparar = true;
	if(bDispararPresionado && ArmaEquipada->bAutomatica) {
		Disparar();
	}

	ArmaVaciaRecargar(); // Recargar automaticamente cuando se queda sin municion en el arma 
}

bool UCombateComponente::PuedoDisparar() {
	return ArmaEquipada != nullptr && !ArmaEquipada->EstaSinMunicion() && bPuedoDisparar && 
			(EstadoCombate == EEstadosCombate::EEC_Desocupado || (EstadoCombate == EEstadosCombate::EEC_Recargando && ArmaEquipada->TipoArmaObtener() == ETipoArma::ETA_Escopeta));
}

void UCombateComponente::GranadaArrojar() {
	if(EstadoCombate != EEstadosCombate::EEC_Desocupado || ArmaEquipada == nullptr) {
		return;
	}

	EstadoCombate = EEstadosCombate::EEC_LanzandoGranada;
	if(DispareitorPersonaje) {
		DispareitorPersonaje->GranadaArrojarMontajeEjecutar();
		ManoIzquierdaUnirActor(ArmaEquipada);
		GranadaMostrar(true);
	}
	if(DispareitorPersonaje && !DispareitorPersonaje->HasAuthority()) {
		GranadaArrojarServidor();
	}
}

void UCombateComponente::GranadaArrojarServidor_Implementation() {
	EstadoCombate = EEstadosCombate::EEC_LanzandoGranada;
	if(DispareitorPersonaje) {
		DispareitorPersonaje->GranadaArrojarMontajeEjecutar();
		ManoIzquierdaUnirActor(ArmaEquipada);
		GranadaMostrar(true);
	}
} 

void UCombateComponente::GranadaMostrar(bool bMostrar) {
	if(DispareitorPersonaje && DispareitorPersonaje->GranadaObtener()) {
		DispareitorPersonaje->GranadaObtener()->SetVisibility(bMostrar);	
	}
}

void UCombateComponente::GranadaArrojada() {
	GranadaMostrar(false);
	if(DispareitorPersonaje && DispareitorPersonaje->HasAuthority() && DispareitorPersonaje->GranadaObtener() && GranadaClase) {
		const FVector PosicionInicial = DispareitorPersonaje->GranadaObtener()->GetComponentLocation();
		FVector AlObjetivo = ObjetoAlcanzado - PosicionInicial;
		FActorSpawnParameters SpawnParametros;
		SpawnParametros.Owner = DispareitorPersonaje;
		SpawnParametros.Instigator = DispareitorPersonaje;
		UWorld* Mundo = GetWorld();
		if(Mundo) {
			Mundo->SpawnActor<AProyectil>(GranadaClase, PosicionInicial, AlObjetivo.Rotation(), SpawnParametros);
		}
	}
}

void UCombateComponente::GranadaArrojarFinalizado() {
	EstadoCombate = EEstadosCombate::EEC_Desocupado;
	ManoDerechaUnirActor(ArmaEquipada);
}


