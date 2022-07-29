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
			FOVPorDefecto = DispareitorPersonaje->ObtenerCamara()->FieldOfView;
			ActualFOV = FOVPorDefecto;
		}
	}

	if(DispareitorPersonaje->HasAuthority()) {
		InicializarMunicionPersonaje();
	}
}

void UCombateComponente::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UCombateComponente, ArmaEquipada);
	DOREPLIFETIME(UCombateComponente, bApuntando);
	DOREPLIFETIME_CONDITION(UCombateComponente, MunicionPersonaje, COND_OwnerOnly);
	DOREPLIFETIME(UCombateComponente, EstadoCombate);
	DOREPLIFETIME(UCombateComponente, GranadasActuales);
}

void UCombateComponente::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if(DispareitorPersonaje && DispareitorPersonaje->IsLocallyControlled()) {
		FHitResult RayoResultado;
		CalcularRayoDesdeCruceta(RayoResultado);
		ObjetoAlcanzado = RayoResultado.ImpactPoint;

		ActualizarCrucetaHUD(DeltaTime);
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

		HUDCruceta.CrucetaColor = RayoResultado.GetActor() && RayoResultado.GetActor()->Implements<UInteractuarConCrucetaInterfaz>() ? FLinearColor::Red : FLinearColor::White;
	}
}

// Llamado por Tick
void UCombateComponente::ActualizarCrucetaHUD(float DeltaTime) {
	if(DispareitorPersonaje == nullptr || DispareitorPersonaje->Controller == nullptr) {
		return;
	}

	DispareitorControladorJugador = DispareitorControladorJugador != nullptr ? DispareitorControladorJugador :  Cast<ADispareitorControladorJugador>(DispareitorPersonaje->Controller); 
	if(DispareitorControladorJugador) {
		DispareitorHUD = DispareitorHUD != nullptr ? DispareitorHUD : Cast<ADispareitorHUD>(DispareitorControladorJugador->GetHUD());
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
			
			CrucetaFactorEnAire = DispareitorPersonaje->GetCharacterMovement()->IsFalling() ? FMath::FInterpTo(CrucetaFactorEnAire, 2.25f, DeltaTime, 2.25f) : FMath::FInterpTo(CrucetaFactorEnAire, 0.f, DeltaTime, 30.f);
			
			CrucetaFactorApuntado = bApuntando ? FMath::FInterpTo(CrucetaFactorApuntado, 0.58f, DeltaTime, 30.f) : FMath::FInterpTo(CrucetaFactorApuntado, 0.f, DeltaTime, 30.f);
			
			CrucetaFactorDisparo = FMath::FInterpTo(CrucetaFactorDisparo, 0.f, DeltaTime, 40.f);
			
			HUDCruceta.CrucetaApertura = 0.5f + CrucetaFactorVelocidad + CrucetaFactorEnAire - CrucetaFactorApuntado + CrucetaFactorDisparo;

			DispareitorHUD->ActualizarCrucetaHUD(HUDCruceta);
		}
	}
}

// Llamado por Tick
void UCombateComponente::InterpolarFOV(float DeltaTime) {
	if(ArmaEquipada == nullptr) {
		return;
	}

	ActualFOV = bApuntando ? FMath::FInterpTo(ActualFOV, ArmaEquipada->ObtenerZoomFOV(), DeltaTime, ArmaEquipada->ObtenerVelocidadInterpolacion()) : 
							FMath::FInterpTo(ActualFOV, FOVPorDefecto, DeltaTime, ZoomVelocidadInterpolacion);

	if(DispareitorPersonaje && DispareitorPersonaje->ObtenerCamara()) {
		DispareitorPersonaje->ObtenerCamara()->SetFieldOfView(ActualFOV);
	}
}

// Llamado en BeginPlay
void UCombateComponente::InicializarMunicionPersonaje() {
	MapaMunicionPersonaje.Emplace(ETipoArma::ETA_RifleAsalto, MunicionPersonajeInicialRifleAsalto);
	MapaMunicionPersonaje.Emplace(ETipoArma::ETA_LanzaCohetes, MunicionPersonajeInicialLanzaCohetes);
	MapaMunicionPersonaje.Emplace(ETipoArma::ETA_Pistola, MunicionPersonajeInicialPistola);
	MapaMunicionPersonaje.Emplace(ETipoArma::ETA_Subfusil, MunicionPersonajeInicialSubfusil);
	MapaMunicionPersonaje.Emplace(ETipoArma::ETA_Escopeta, MunicionPersonajeInicialEscopeta);
	MapaMunicionPersonaje.Emplace(ETipoArma::ETA_Francotirador, MunicionPersonajeInicialFrancotirador);
	MapaMunicionPersonaje.Emplace(ETipoArma::ETA_LanzaGranadas, MunicionPersonajeInicialLanzaGranadas);
}

// Llamado por ADispareitorPersonaje::Equipar 
// Solo ejecutado en el servidor
void UCombateComponente::EquiparArma(class AArma* ArmaAEquipar) {
	if(DispareitorPersonaje == nullptr || ArmaAEquipar == nullptr || EstadoCombate != EEstadosCombate::EEC_Desocupado) {
		return;
	}

	SoltarArmaEquipada();
	ArmaEquipada = ArmaAEquipar;
	ArmaEquipada->ActualizarEstado(EEstado::EEA_Equipada); // Se propaga al cliente
	ManoDerechaUnirAActor(ArmaEquipada);
	ArmaEquipada->SetOwner(DispareitorPersonaje);	
	ArmaEquipada->ActualizarMunicionHUD();
	ActualizarMunicionPersonaje();
	EjecutarSonidoAlEquipar();
	RecargarArmaVacia();

	DispareitorPersonaje->GetCharacterMovement()->bOrientRotationToMovement = false;
	DispareitorPersonaje->bUseControllerRotationYaw = true;
}

void UCombateComponente::SoltarArmaEquipada() {
	if(ArmaEquipada) {
		ArmaEquipada->Soltar();
	}
}

void UCombateComponente::ManoDerechaUnirAActor(AActor* Actor) {
	if(DispareitorPersonaje == nullptr || DispareitorPersonaje->GetMesh() == nullptr || Actor == nullptr) {
		return;	
	}  

	const USkeletalMeshSocket* ManoDerechaSocket = DispareitorPersonaje->GetMesh()->GetSocketByName(FName("ManoDerechaSocket"));
	if(ManoDerechaSocket) {
		ManoDerechaSocket->AttachActor(Actor, DispareitorPersonaje->GetMesh()); // Tambien se propaga a los clientes, pero no hay garantias de cual se propaga antes 
	}
}

void UCombateComponente::ManoIzquierdaUnirAActor(AActor* Actor) {
	if(DispareitorPersonaje == nullptr || DispareitorPersonaje->GetMesh() == nullptr || Actor == nullptr) {
		return;	
	}  

	const USkeletalMeshSocket* ManoIzquierdaSocket = DispareitorPersonaje->GetMesh()->GetSocketByName(FName("ManoIzquierdaSocket"));
	if(ManoIzquierdaSocket) {
		ManoIzquierdaSocket->AttachActor(Actor, DispareitorPersonaje->GetMesh()); // Tambien se propaga a los clientes, pero no hay garantias de cual se propaga antes 
	}
}

void UCombateComponente::RecargarArmaVacia() {
	if(ArmaEquipada && ArmaEquipada->EstaSinMunicion()) {
		Recargar();
	}
}

void UCombateComponente::ActualizarMunicionPersonaje() {
	if(ArmaEquipada == nullptr) {
		return;
	}

	if(MapaMunicionPersonaje.Contains(ArmaEquipada->ObtenerTipoArma())) {
		MunicionPersonaje = MapaMunicionPersonaje[ArmaEquipada->ObtenerTipoArma()];
	}

	DispareitorControladorJugador = DispareitorControladorJugador != nullptr ? DispareitorControladorJugador : Cast<ADispareitorControladorJugador>(DispareitorPersonaje->Controller);
	if(DispareitorControladorJugador) {
		DispareitorControladorJugador->ActualizarMunicionPersonajeHUD(MunicionPersonaje);
	}
}


void UCombateComponente::AlReplicar_ArmaEquipada() {
	if(ArmaEquipada && DispareitorPersonaje) {
		// Para garantizar que se ejecutan en orden las ejecutamos en los clientes en el orden correcto
		ArmaEquipada->ActualizarEstado(EEstado::EEA_Equipada); 
		ManoDerechaUnirAActor(ArmaEquipada);
		DispareitorPersonaje->GetCharacterMovement()->bOrientRotationToMovement = false;
		DispareitorPersonaje->bUseControllerRotationYaw = true;
		EjecutarSonidoAlEquipar();
	}
}

void UCombateComponente::EjecutarSonidoAlEquipar() {
	if(DispareitorPersonaje && ArmaEquipada && ArmaEquipada->SonidoEquipar) {
		UGameplayStatics::PlaySoundAtLocation(this, ArmaEquipada->SonidoEquipar, DispareitorPersonaje->GetActorLocation());
	}
}

// Llamado por ADispareitorPersonaje::RecargarPulsado
// Puede ser llamado tanto en un cliente como en un servidor
void UCombateComponente::Recargar() {
	if(ArmaEquipada == nullptr || ArmaEquipada->EstaConMunicionLlena()) {
		return;
	}

	// Si lo ejecutamos desde el cliente podemos chequear si tiene municion para evitar llamadas innecesarias al servidor
	if(MunicionPersonaje > 0 && EstadoCombate == EEstadosCombate::EEC_Desocupado) {
		Recargar_EnServidor();
	}
}

void UCombateComponente::Recargar_EnServidor_Implementation() {
	EstadoCombate = EEstadosCombate::EEC_Recargando;
	EjecutarMontajeRecargar();
}

// Llamado por RecargarServidor_Implementation (para el servidor) y AlReplicar_EstadoCombate (para los clientes)
void UCombateComponente::EjecutarMontajeRecargar() {
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
		ActualizarValoresMunicion();
	}

	if(bDispararPresionado) {
		Disparar();
	}
}

// Llamado por BP_DispareitorInstanciaAnimacion al ejecutar una notificacion en Montaje_Recargar al insertar un cartucho en la escopeta
void UCombateComponente::RecargarCartuchoEscopeta() {
	if(DispareitorPersonaje && DispareitorPersonaje->HasAuthority()) {
		ActualizarValoresMunicionEscopeta();
	}
}

void UCombateComponente::ActualizarValoresMunicion() {
	if(DispareitorPersonaje == nullptr || ArmaEquipada == nullptr || !MapaMunicionPersonaje.Contains(ArmaEquipada->ObtenerTipoArma())) {
		return;
	}

	int32 RecargarCantidadValor = CalcularCantidadARecargar();
	MapaMunicionPersonaje[ArmaEquipada->ObtenerTipoArma()] -= RecargarCantidadValor;
	MunicionPersonaje = MapaMunicionPersonaje[ArmaEquipada->ObtenerTipoArma()];
	ArmaEquipada->AniadirMunicion(RecargarCantidadValor);

	DispareitorControladorJugador = DispareitorControladorJugador != nullptr ? DispareitorControladorJugador : Cast<ADispareitorControladorJugador>(DispareitorPersonaje->Controller);
	if(DispareitorControladorJugador) {
		DispareitorControladorJugador->ActualizarMunicionPersonajeHUD(MunicionPersonaje);
	}
}

void UCombateComponente::ActualizarValoresMunicionEscopeta() {
	if(DispareitorPersonaje == nullptr || ArmaEquipada == nullptr || !MapaMunicionPersonaje.Contains(ArmaEquipada->ObtenerTipoArma())) {
		return;
	}

	MapaMunicionPersonaje[ArmaEquipada->ObtenerTipoArma()] -= 1;
	MunicionPersonaje = MapaMunicionPersonaje[ArmaEquipada->ObtenerTipoArma()];
	DispareitorControladorJugador = DispareitorControladorJugador != nullptr ? DispareitorControladorJugador : Cast<ADispareitorControladorJugador>(DispareitorPersonaje->Controller);
	if(DispareitorControladorJugador) {
		DispareitorControladorJugador->ActualizarMunicionPersonajeHUD(MunicionPersonaje);
	}
	ArmaEquipada->AniadirMunicion(1);
	bPuedoDisparar = true;
	if(ArmaEquipada->EstaConMunicionLlena() || MunicionPersonaje == 0) {
		SaltarAFinAnimacionEscopeta();
	}
}

void UCombateComponente::SaltarAFinAnimacionEscopeta() {
	UAnimInstance* InstanciaAnimacion = DispareitorPersonaje->GetMesh()->GetAnimInstance();
	if(InstanciaAnimacion && DispareitorPersonaje->ObtenerMontajeRecargar()) {
		InstanciaAnimacion->Montage_JumpToSection("EscopetaFin");
	}
}

void UCombateComponente::AlReplicar_MunicionPersonaje() {
	DispareitorControladorJugador = DispareitorControladorJugador != nullptr ? DispareitorControladorJugador : Cast<ADispareitorControladorJugador>(DispareitorPersonaje->Controller);
	if(DispareitorControladorJugador) {
		DispareitorControladorJugador->ActualizarMunicionPersonajeHUD(MunicionPersonaje);
	}

	if(EstadoCombate == EEstadosCombate::EEC_Recargando && ArmaEquipada != nullptr && ArmaEquipada->ObtenerTipoArma() == ETipoArma::ETA_Escopeta && MunicionPersonaje == 0) {
		SaltarAFinAnimacionEscopeta();
	}
}


int32 UCombateComponente::CalcularCantidadARecargar() {
	if(ArmaEquipada == nullptr || !MapaMunicionPersonaje.Contains(ArmaEquipada->ObtenerTipoArma())) {
		return 0;
	}

	int32 CargadorEspacio = ArmaEquipada->ObtenerCapacidadCargador() - ArmaEquipada->ObtenerMunicion();
	int32 MunicionPersonajeTemporal = MapaMunicionPersonaje[ArmaEquipada->ObtenerTipoArma()];
	int32 Minimo = FMath::Min(CargadorEspacio, MunicionPersonajeTemporal);
	return FMath::Clamp(CargadorEspacio, 0, Minimo);
}


void UCombateComponente::AlReplicar_EstadoCombate() {
	switch(EstadoCombate) {
		case EEstadosCombate::EEC_Recargando:
			EjecutarMontajeRecargar();
			break;
		case EEstadosCombate::EEC_Desocupado:
			if(bDispararPresionado) {
				Disparar();
			}
			break;	
		case EEstadosCombate::EEC_LanzandoGranada:
			if(DispareitorPersonaje && !DispareitorPersonaje->IsLocallyControlled()) {
				DispareitorPersonaje->EjecutarMontajeArrojarGranada();
				ManoIzquierdaUnirAActor(ArmaEquipada);
				MostrarGranada(true);
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
	ActualizarApuntando_EnServidor(bApuntando);

	if(DispareitorPersonaje) {
		DispareitorPersonaje->GetCharacterMovement()->MaxWalkSpeed = bApuntando ? VelocidadCaminarApuntando : VelocidadCaminarBase;

		if (DispareitorPersonaje->IsLocallyControlled() && ArmaEquipada->ObtenerTipoArma() == ETipoArma::ETA_Francotirador) {
			DispareitorControladorJugador = DispareitorControladorJugador != nullptr ? DispareitorControladorJugador :  Cast<ADispareitorControladorJugador>(DispareitorPersonaje->Controller);
			if (DispareitorControladorJugador) {
				DispareitorControladorJugador->ActualizarCrucetaFrancotiradorHUD(Apuntando);
				if (Apuntando && SonidoFrancotiradorCrucetaZoomIn) {
					UGameplayStatics::PlaySound2D(this, SonidoFrancotiradorCrucetaZoomIn);
				} else if(SonidoFrancotiradorCrucetaZoomOut) {
					UGameplayStatics::PlaySound2D(this, SonidoFrancotiradorCrucetaZoomOut);
				}
			}
		}
	}
}

void UCombateComponente::ActualizarApuntando_EnServidor_Implementation(bool Apuntando) {
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
		Disparar_EnServidor(ObjetoAlcanzado);

		if(ArmaEquipada) {
			CrucetaFactorDisparo = 0.75f;
		}

		EmpezarTemporizadorDisparo();
	}  
}

// Esta funcion solo se ejecutará en el servidor
void UCombateComponente::Disparar_EnServidor_Implementation(const FVector_NetQuantize& Objetivo) {
	Disparar_Multicast(Objetivo);
}

// Esta función se ejecutará en el servidor + clientes
void UCombateComponente::Disparar_Multicast_Implementation(const FVector_NetQuantize& Objetivo) {
	if(DispareitorPersonaje && ArmaEquipada) {
		if(EstadoCombate == EEstadosCombate::EEC_Desocupado) {
			DispareitorPersonaje->EjecutarMontajeDispararArma(bApuntando);
			ArmaEquipada->Disparar(Objetivo);
		} else if(EstadoCombate == EEstadosCombate::EEC_Recargando && ArmaEquipada->ObtenerTipoArma() == ETipoArma::ETA_Escopeta) {
			DispareitorPersonaje->EjecutarMontajeDispararArma(bApuntando);
			ArmaEquipada->Disparar(Objetivo);
			EstadoCombate = EEstadosCombate::EEC_Desocupado;
		}
	} 
}

void UCombateComponente::EmpezarTemporizadorDisparo() {
	if(DispareitorPersonaje == nullptr || ArmaEquipada == nullptr) {
		return;
	}

	DispareitorPersonaje->GetWorldTimerManager().SetTimer(TemporizadorDisparo, this, &UCombateComponente::TerminadoDisparoTemporizador, ArmaEquipada->RetardoAlDisparar);
}

void UCombateComponente::TerminadoDisparoTemporizador() {
	if(DispareitorPersonaje == nullptr || ArmaEquipada == nullptr) {
		return;
	}

	bPuedoDisparar = true;
	if(bDispararPresionado && ArmaEquipada->bAutomatica) {
		Disparar();
	}

	RecargarArmaVacia(); // Recargar automaticamente cuando se queda sin municion en el arma 
}

bool UCombateComponente::PuedoDisparar() {
	return ArmaEquipada != nullptr && !ArmaEquipada->EstaSinMunicion() && bPuedoDisparar && 
			(EstadoCombate == EEstadosCombate::EEC_Desocupado || (EstadoCombate == EEstadosCombate::EEC_Recargando && ArmaEquipada->ObtenerTipoArma() == ETipoArma::ETA_Escopeta));
}

// Llamado por ADispareitorPersonaje::ArrojarGranadaPulsado
void UCombateComponente::ArrojarGranada() {
	if(GranadasActuales == 0 || EstadoCombate != EEstadosCombate::EEC_Desocupado || ArmaEquipada == nullptr) {
		return;
	}

	EstadoCombate = EEstadosCombate::EEC_LanzandoGranada;
	if(DispareitorPersonaje) {
		DispareitorPersonaje->EjecutarMontajeArrojarGranada();
		ManoIzquierdaUnirAActor(ArmaEquipada);
		MostrarGranada(true);

		if(!DispareitorPersonaje->HasAuthority()) {
			ArrojarGranada_EnServidor();
		} else {
			GranadasActuales = FMath::Clamp(GranadasActuales - 1, 0, GranadasMaximo);
			ActualizarGranadasHUD();	
		}
	}
}

void UCombateComponente::ArrojarGranada_EnServidor_Implementation() {
	if(GranadasActuales == 0) {
		return;
	}

	EstadoCombate = EEstadosCombate::EEC_LanzandoGranada;
	if(DispareitorPersonaje) {
		DispareitorPersonaje->EjecutarMontajeArrojarGranada();
		ManoIzquierdaUnirAActor(ArmaEquipada);
		MostrarGranada(true);
	}

	GranadasActuales = FMath::Clamp(GranadasActuales - 1, 0, GranadasMaximo);
	ActualizarGranadasHUD();
} 

void UCombateComponente::AlReplicar_GranadasActuales() {
	ActualizarGranadasHUD();
}

void UCombateComponente::ActualizarGranadasHUD() {
	DispareitorControladorJugador = DispareitorControladorJugador == nullptr ? Cast<ADispareitorControladorJugador>(DispareitorPersonaje->Controller) : DispareitorControladorJugador; 
	if(DispareitorControladorJugador) {
		DispareitorControladorJugador->ActualizarGranadasHUD(GranadasActuales);
	}
}

void UCombateComponente::MostrarGranada(bool bMostrar) {
	if(DispareitorPersonaje && DispareitorPersonaje->ObtenerGranada()) {
		DispareitorPersonaje->ObtenerGranada()->SetVisibility(bMostrar);	
	}
}

void UCombateComponente::GranadaArrojada() {
	MostrarGranada(false);
	if(DispareitorPersonaje && DispareitorPersonaje->IsLocallyControlled()) {
		GranadaArrojada_EnServidor(ObjetoAlcanzado);
	}
}

void UCombateComponente::GranadaArrojada_EnServidor_Implementation(const FVector_NetQuantize& Objetivo) {
	if(DispareitorPersonaje && DispareitorPersonaje->ObtenerGranada() && GranadaClase) {
		const FVector PosicionInicial = DispareitorPersonaje->ObtenerGranada()->GetComponentLocation() + 40.f; // Le sumamos 40 para evitar chocar contra la propia malla del personaje o de su arma
		FVector AlObjetivo = Objetivo - PosicionInicial;
		FActorSpawnParameters SpawnParametros;
		SpawnParametros.Owner = DispareitorPersonaje;
		SpawnParametros.Instigator = DispareitorPersonaje;
		UWorld* Mundo = GetWorld();
		if(Mundo) {
			Mundo->SpawnActor<AProyectil>(GranadaClase, PosicionInicial, AlObjetivo.Rotation(), SpawnParametros);
		}
	}
}

void UCombateComponente::ArrojarGranadaFinalizado() {
	EstadoCombate = EEstadosCombate::EEC_Desocupado;
	ManoDerechaUnirAActor(ArmaEquipada);
}

void UCombateComponente::CogerMunicion(ETipoArma TipoArma, int32 IncrementoMunicion) {
	if(MapaMunicionPersonaje.Contains(TipoArma)) {
		MapaMunicionPersonaje[TipoArma] = FMath::Clamp(MapaMunicionPersonaje[TipoArma] + IncrementoMunicion, 0, MaximaMunicionPersonaje);	
		ActualizarMunicionPersonaje();
	}	
	if(ArmaEquipada && ArmaEquipada->EstaSinMunicion() && ArmaEquipada->ObtenerTipoArma() == TipoArma) {
		Recargar();
	}
}


