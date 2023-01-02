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
#include "Dispareitor/Arma/Escopeta.h"
#include "Dispareitor/ModoJuego/DispareitorModoJuego.h"

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
	DOREPLIFETIME(UCombateComponente, ArmaSecundariaEquipada);
	DOREPLIFETIME(UCombateComponente, bApuntando);
	DOREPLIFETIME_CONDITION(UCombateComponente, MunicionPersonaje, COND_OwnerOnly);
	DOREPLIFETIME(UCombateComponente, EstadoCombate);
	DOREPLIFETIME(UCombateComponente, GranadasActuales);
	DOREPLIFETIME(UCombateComponente, bSosteniendoBandera);	
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
	if(!DispareitorPersonaje || !DispareitorPersonaje->Controller) {
		return;
	}

	DispareitorControladorJugador = DispareitorControladorJugador ? DispareitorControladorJugador :  Cast<ADispareitorControladorJugador>(DispareitorPersonaje->Controller); 
	if(DispareitorControladorJugador) {
		DispareitorHUD = DispareitorHUD ? DispareitorHUD : Cast<ADispareitorHUD>(DispareitorControladorJugador->GetHUD());
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
	if(!ArmaEquipada) {
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
	if(!DispareitorPersonaje || !ArmaAEquipar || EstadoCombate != EEstadosCombate::EEC_Desocupado) {
		return;
	}

	if(ArmaAEquipar->ObtenerTipoArma() == ETipoArma::ETA_Bandera) {
		DispareitorPersonaje->Crouch();
		bSosteniendoBandera = true;
		ArmaAEquipar->ActualizarEstado(EEstado::EEA_Equipada);		
		UnirBanderaAManoIzquierda(ArmaAEquipar);
		ArmaAEquipar->SetOwner(DispareitorPersonaje);
		ArmaBandera = ArmaAEquipar;
	} else {
		if(ArmaEquipada && !ArmaSecundariaEquipada) {
			EquiparArmaSecundaria(ArmaAEquipar);					
		} else {
			EquiparArmaPrimaria(ArmaAEquipar);			
		}

		ADispareitorModoJuego* DModoJuego = GetWorld()->GetAuthGameMode<ADispareitorModoJuego>();
		if(DModoJuego && !ArmaAEquipar->bDestruirArma) {
			//UE_LOG(LogTemp, Warning, TEXT("UCombateComponente::EquiparArma. NombrePuntoReaparicion: %s"), *ArmaAEquipar->ObtenerNombrePuntoReaparicion());
			DModoJuego->ActualizarPuntoReaparicionArmaALibre(*ArmaAEquipar->ObtenerNombrePuntoReaparicion());				
		}	

		DispareitorPersonaje->GetCharacterMovement()->bOrientRotationToMovement = false;
		DispareitorPersonaje->bUseControllerRotationYaw = true;
	}	
}

void UCombateComponente::EquiparArmaPrimaria(AArma* ArmaAEquipar) {
	if(!ArmaAEquipar) {
		return; 
	}

	SoltarArmaEquipada();
	ArmaEquipada = ArmaAEquipar;
	ArmaEquipada->ActualizarEstado(EEstado::EEA_Equipada); // Se propaga al cliente
	UnirActorAManoDerecha(ArmaEquipada);
	ArmaEquipada->SetOwner(DispareitorPersonaje);	
	ArmaEquipada->ActualizarMunicionHUD();
	ActualizarMunicionPersonaje();
	EjecutarSonidoAlEquipar(ArmaAEquipar);
	RecargarArmaVacia();
}

void UCombateComponente::AlReplicar_ArmaEquipada() {
	if(ArmaEquipada && DispareitorPersonaje) {
		// Para garantizar que se ejecutan en orden las ejecutamos en los clientes en el orden correcto
		ArmaEquipada->ActualizarEstado(EEstado::EEA_Equipada); 
		UnirActorAManoDerecha(ArmaEquipada);
		DispareitorPersonaje->GetCharacterMovement()->bOrientRotationToMovement = false;
		DispareitorPersonaje->bUseControllerRotationYaw = true;
		EjecutarSonidoAlEquipar(ArmaEquipada);
		ArmaEquipada->ActualizarMunicionHUD();
	}
}

void UCombateComponente::EquiparArmaSecundaria(AArma* ArmaAEquipar) {
	if(!ArmaAEquipar) {
		return; 
	}

	ArmaSecundariaEquipada = ArmaAEquipar;
	ArmaSecundariaEquipada->ActualizarEstado(EEstado::EEA_EquipadaSecundaria); 
	UnirActorAMochila(ArmaAEquipar);
	ArmaSecundariaEquipada->SetOwner(DispareitorPersonaje);	
	EjecutarSonidoAlEquipar(ArmaAEquipar);
}

void UCombateComponente::AlReplicar_ArmaSecundariaEquipada() {
	if(ArmaSecundariaEquipada && DispareitorPersonaje) {
		ArmaSecundariaEquipada->ActualizarEstado(EEstado::EEA_EquipadaSecundaria); 
		UnirActorAMochila(ArmaSecundariaEquipada);
		EjecutarSonidoAlEquipar(ArmaSecundariaEquipada);
	}
}

void UCombateComponente::IntercambiarArmas() {
	if(!DispareitorPersonaje) {
		return;	
	}  

	DispareitorPersonaje->EjecutarMontajeIntercambiarArmas();
	DispareitorPersonaje->bIntercambiarArmasFinalizado = false;
	EstadoCombate = EEstadosCombate::EEC_IntercambiandoArmas;

	AArma* ArmaTemporal = ArmaEquipada;
	ArmaEquipada = ArmaSecundariaEquipada;
	ArmaSecundariaEquipada = ArmaTemporal;
}

void UCombateComponente::SoltarArmaEquipada() {
	if(ArmaEquipada) {
		ArmaEquipada->Soltar();
	}
}

void UCombateComponente::UnirActorAManoDerecha(AActor* Actor) {
	if(!DispareitorPersonaje || !DispareitorPersonaje->GetMesh() || !Actor) {
		return;	
	}  

	const USkeletalMeshSocket* ManoDerechaSocket = DispareitorPersonaje->GetMesh()->GetSocketByName(FName("ManoDerechaSocket"));
	if(ManoDerechaSocket) {
		ManoDerechaSocket->AttachActor(Actor, DispareitorPersonaje->GetMesh()); // Tambien se propaga a los clientes, pero no hay garantias de cual se propaga antes 
	}
}

void UCombateComponente::UnirActorAManoIzquierda(AActor* Actor) {
	if(!DispareitorPersonaje || !DispareitorPersonaje->GetMesh() || !Actor) {
		return;	
	}  

	const USkeletalMeshSocket* ManoIzquierdaSocket = DispareitorPersonaje->GetMesh()->GetSocketByName(FName("ManoIzquierdaSocket"));
	if(ManoIzquierdaSocket) {
		ManoIzquierdaSocket->AttachActor(Actor, DispareitorPersonaje->GetMesh()); // Tambien se propaga a los clientes, pero no hay garantias de cual se propaga antes 
	}
}

void UCombateComponente::UnirBanderaAManoIzquierda(AArma* Bandera) {
	if(!DispareitorPersonaje || !DispareitorPersonaje->GetMesh() || !Bandera) {
		return;	
	}  

	const USkeletalMeshSocket* BanderaSocket = DispareitorPersonaje->GetMesh()->GetSocketByName(FName("BanderaSocket"));
	if(BanderaSocket) {
		BanderaSocket->AttachActor(Bandera, DispareitorPersonaje->GetMesh()); 
	}
}

void UCombateComponente::UnirActorAMochila(AActor* Actor) {
	if(!DispareitorPersonaje || !DispareitorPersonaje->GetMesh() || !Actor) {
		return;	
	}  

	const USkeletalMeshSocket* MochilaSocket = DispareitorPersonaje->GetMesh()->GetSocketByName(FName("MochilaSocket"));
	if(MochilaSocket) {
		MochilaSocket->AttachActor(Actor, DispareitorPersonaje->GetMesh()); 
	}
}

void UCombateComponente::RecargarArmaVacia() {
	if(ArmaEquipada && ArmaEquipada->EstaSinMunicion()) {
		Recargar();
	}
}

void UCombateComponente::ActualizarMunicionPersonaje() {
	if(!ArmaEquipada) {
		return;
	}

	if(MapaMunicionPersonaje.Contains(ArmaEquipada->ObtenerTipoArma())) {
		MunicionPersonaje = MapaMunicionPersonaje[ArmaEquipada->ObtenerTipoArma()];
	}

	DispareitorControladorJugador = DispareitorControladorJugador ? DispareitorControladorJugador : Cast<ADispareitorControladorJugador>(DispareitorPersonaje->Controller);
	if(DispareitorControladorJugador) {
		DispareitorControladorJugador->ActualizarMunicionPersonajeHUD(MunicionPersonaje);
	}
}

void UCombateComponente::EjecutarSonidoAlEquipar(AArma* ArmaAEquipar) {
	if(DispareitorPersonaje && ArmaAEquipar && ArmaAEquipar->SonidoEquipar) {
		UGameplayStatics::PlaySoundAtLocation(this, ArmaAEquipar->SonidoEquipar, DispareitorPersonaje->GetActorLocation());
	}
}

// Llamado por ADispareitorPersonaje::RecargarPulsado
// Puede ser llamado tanto en un cliente como en un servidor
void UCombateComponente::Recargar() {
	// Si lo ejecutamos desde el cliente podemos chequear si tiene municion para evitar llamadas innecesarias al servidor
	if(ArmaEquipada && !ArmaEquipada->EstaConMunicionLlena() && MunicionPersonaje > 0 && EstadoCombate == EEstadosCombate::EEC_Desocupado && !bRecargandoLocalmente) {
		Recargar_EnServidor();
		EjecutarMontajeRecargar();
		bRecargandoLocalmente = true;
	}
}

void UCombateComponente::Recargar_EnServidor_Implementation() {
	if(!DispareitorPersonaje || !ArmaEquipada) {
		return;
	}
	EstadoCombate = EEstadosCombate::EEC_Recargando;
	if(!DispareitorPersonaje->IsLocallyControlled()) {
		EjecutarMontajeRecargar();
	}
}

// Llamado por RecargarServidor_Implementation (para el servidor) y AlReplicar_EstadoCombate (para los clientes)
void UCombateComponente::EjecutarMontajeRecargar() {
	if(DispareitorPersonaje) {
		DispareitorPersonaje->EjecutarMontajeRecargar();
	}
}

// Llamado por BP_DispareitorInstanciaAnimacion al ejecutar una notificacion en Montaje_Recargar
void UCombateComponente::RecargarFinalizado() {
	if(!DispareitorPersonaje) {
		return;
	}

	bRecargandoLocalmente = false;

	if(DispareitorPersonaje->HasAuthority()) {
		EstadoCombate = EEstadosCombate::EEC_Desocupado;
		ActualizarValoresMunicion();
	}

	if(bDispararPresionado) {
		Disparar();
	}
}

void UCombateComponente::ActualizarValoresMunicion() {
	if(!DispareitorPersonaje || !ArmaEquipada || !MapaMunicionPersonaje.Contains(ArmaEquipada->ObtenerTipoArma())) {
		return;
	}

	int32 RecargarCantidadValor = CalcularCantidadARecargar();
	UE_LOG(LogTemp, Warning, TEXT("UCombateComponente::ActualizarValoresMunicion. RecargarCantidadValor: %d"), RecargarCantidadValor);
	MapaMunicionPersonaje[ArmaEquipada->ObtenerTipoArma()] -= RecargarCantidadValor;
	MunicionPersonaje = MapaMunicionPersonaje[ArmaEquipada->ObtenerTipoArma()];
	ArmaEquipada->AniadirMunicion(RecargarCantidadValor);

	DispareitorControladorJugador = DispareitorControladorJugador ? DispareitorControladorJugador : Cast<ADispareitorControladorJugador>(DispareitorPersonaje->Controller);
	if(DispareitorControladorJugador) {
		DispareitorControladorJugador->ActualizarMunicionPersonajeHUD(MunicionPersonaje);
	}
}

// Llamado por BP_DispareitorInstanciaAnimacion al ejecutar una notificacion en Montaje_Recargar al insertar un cartucho en la escopeta
void UCombateComponente::RecargarCartuchoEscopeta() {
	if(DispareitorPersonaje && DispareitorPersonaje->HasAuthority()) {
		ActualizarValoresMunicionEscopeta();
	}
}

void UCombateComponente::ActualizarValoresMunicionEscopeta() {
	if(!DispareitorPersonaje || !ArmaEquipada || !MapaMunicionPersonaje.Contains(ArmaEquipada->ObtenerTipoArma())) {
		return;
	}

	MapaMunicionPersonaje[ArmaEquipada->ObtenerTipoArma()] -= 1;
	MunicionPersonaje = MapaMunicionPersonaje[ArmaEquipada->ObtenerTipoArma()];
	DispareitorControladorJugador = DispareitorControladorJugador ? DispareitorControladorJugador : Cast<ADispareitorControladorJugador>(DispareitorPersonaje->Controller);
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
	DispareitorControladorJugador = DispareitorControladorJugador ? DispareitorControladorJugador : Cast<ADispareitorControladorJugador>(DispareitorPersonaje->Controller);
	if(DispareitorControladorJugador) {
		DispareitorControladorJugador->ActualizarMunicionPersonajeHUD(MunicionPersonaje);
	}

	if(EstadoCombate == EEstadosCombate::EEC_Recargando && ArmaEquipada && ArmaEquipada->ObtenerTipoArma() == ETipoArma::ETA_Escopeta && MunicionPersonaje == 0) {
		SaltarAFinAnimacionEscopeta();
	}
}


int32 UCombateComponente::CalcularCantidadARecargar() {
	if(!ArmaEquipada || !MapaMunicionPersonaje.Contains(ArmaEquipada->ObtenerTipoArma())) {
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
			if(DispareitorPersonaje && !DispareitorPersonaje->IsLocallyControlled()) {
				EjecutarMontajeRecargar();
			}
			break;
		case EEstadosCombate::EEC_Desocupado:
			if(bDispararPresionado) {
				Disparar();
			}
			break;	
		case EEstadosCombate::EEC_LanzandoGranada:
			if(DispareitorPersonaje && !DispareitorPersonaje->IsLocallyControlled()) {
				DispareitorPersonaje->EjecutarMontajeArrojarGranada();
				UnirActorAManoIzquierda(ArmaEquipada);
				MostrarGranada(true);
			}
			break;	
		case EEstadosCombate::EEC_IntercambiandoArmas:
			if(DispareitorPersonaje && !DispareitorPersonaje->IsLocallyControlled()) {
				DispareitorPersonaje->EjecutarMontajeIntercambiarArmas();
			}
			break;		
	}
}

// Llamado por DispareitorPersonaje cuando se pulsa o libera el boton de apuntar
void UCombateComponente::ActualizarApuntando(bool Apuntando) {
	if(!DispareitorPersonaje || !ArmaEquipada) {
		return;
	}

	// Aunque esta funcion es posible que la llamemos desde el cliente, por cuestiones cosmeticas podemos hacerlo ahora
	bApuntando = Apuntando; 
	// Si lo estamos ejecutando en un cliente se invocará esta función en el servidor, y si lo estamos ejecutando en el servidor se ejecutará en él mismo
	ActualizarApuntando_EnServidor(bApuntando);

	if(DispareitorPersonaje) {
		DispareitorPersonaje->GetCharacterMovement()->MaxWalkSpeed = bApuntando ? VelocidadCaminarApuntando : VelocidadCaminarBase;

		if (DispareitorPersonaje->IsLocallyControlled()) {
			bBotonApuntadoPresionado = bApuntando;
			if(ArmaEquipada->ObtenerTipoArma() == ETipoArma::ETA_Francotirador) {
				DispareitorControladorJugador = DispareitorControladorJugador ? DispareitorControladorJugador :  Cast<ADispareitorControladorJugador>(DispareitorPersonaje->Controller);
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
}

void UCombateComponente::AlReplicar_Apuntando() {
	if (DispareitorPersonaje && DispareitorPersonaje->IsLocallyControlled()) {
		bApuntando = bBotonApuntadoPresionado;
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
		bRecargandoLocalmente = false; // Porque puede que hayamos interrumpido la recargar de la escopeta al disparar en cuyo caso debemos debemos poner esta variable a false (que se pone a false al final de la recargar)
		
		if(ArmaEquipada) {
			CrucetaFactorDisparo = 0.75f;
			switch(ArmaEquipada->TipoDisparo) {
				case ETipoDisparo::ETD_Proyectil:
					DispararArmaProyectil();
				break;
				case ETipoDisparo::ETD_HitScan:
					DispararArmaHitScan();
				break;
				case ETipoDisparo::ETD_Escopeta:
					DispararArmaEscopeta();
				break;
			}
		}

		EmpezarTemporizadorDisparo();
	}  
}

void UCombateComponente::DispararArmaProyectil() {
	if(ArmaEquipada && DispareitorPersonaje) {
		ObjetoAlcanzado = ArmaEquipada->bUsarDispersion ? ArmaEquipada->CalcularPuntoFinalConDispersion(ObjetoAlcanzado) : ObjetoAlcanzado;
		if(!DispareitorPersonaje->HasAuthority()) {
			DispararLocalmente(ObjetoAlcanzado);
		}
		// Si estamos en el server se ejecutar en el server y si estamos en un cliente se ejectura en el server
		Disparar_EnServidor(ObjetoAlcanzado);
	}
}

void UCombateComponente::DispararArmaHitScan() {
	if(ArmaEquipada && DispareitorPersonaje) {
		ObjetoAlcanzado = ArmaEquipada->bUsarDispersion ? ArmaEquipada->CalcularPuntoFinalConDispersion(ObjetoAlcanzado) : ObjetoAlcanzado;
		if(!DispareitorPersonaje->HasAuthority()) {
			DispararLocalmente(ObjetoAlcanzado);
		}
		Disparar_EnServidor(ObjetoAlcanzado);
	}
}

void UCombateComponente::DispararArmaEscopeta() {
	AEscopeta* Escopeta = Cast<AEscopeta>(ArmaEquipada); 
	if(Escopeta && DispareitorPersonaje) {
		TArray<FVector_NetQuantize> Objetivos;
		Escopeta->CalcularPuntosFinalesConDispersionParaEscopeta(ObjetoAlcanzado, Objetivos);
		if(!DispareitorPersonaje->HasAuthority()) {
			DispararEscopetaLocalmente(Objetivos);
		}
		DispararEscopeta_EnServidor(Objetivos);
	}
}

// Esta funcion solo se ejecutará en el servidor
void UCombateComponente::Disparar_EnServidor_Implementation(const FVector_NetQuantize& Objetivo) {
	Disparar_Multicast(Objetivo);
}

// Esta función se ejecutará en el servidor + clientes
void UCombateComponente::Disparar_Multicast_Implementation(const FVector_NetQuantize& Objetivo) {
	if(DispareitorPersonaje && DispareitorPersonaje->IsLocallyControlled() && !DispareitorPersonaje->HasAuthority()) {
		return;
	} 
	DispararLocalmente(Objetivo); // Estamos en el servidor o en un cliente que no controla este personaje
}

void UCombateComponente::DispararEscopeta_EnServidor_Implementation(const TArray<FVector_NetQuantize>& Objetivos) {
	DispararEscopeta_Multicast(Objetivos);
}

void UCombateComponente::DispararEscopeta_Multicast_Implementation(const TArray<FVector_NetQuantize>& Objetivos) {
	if(DispareitorPersonaje && DispareitorPersonaje->IsLocallyControlled() && !DispareitorPersonaje->HasAuthority()) {
		return;
	} 
	DispararEscopetaLocalmente(Objetivos);
}
	
void UCombateComponente::DispararLocalmente(const FVector_NetQuantize& Objetivo) {
	if(DispareitorPersonaje && ArmaEquipada) {
		if(EstadoCombate == EEstadosCombate::EEC_Desocupado) {
			DispareitorPersonaje->EjecutarMontajeDispararArma(bApuntando);
			ArmaEquipada->Disparar(Objetivo);
		} 
	} 
}

void UCombateComponente::DispararEscopetaLocalmente(const TArray<FVector_NetQuantize>& Objetivos) {
	AEscopeta* Escopeta = Cast<AEscopeta>(ArmaEquipada);
	if(DispareitorPersonaje && Escopeta) {
		if(EstadoCombate == EEstadosCombate::EEC_Recargando || EstadoCombate == EEstadosCombate::EEC_Desocupado) {
			DispareitorPersonaje->EjecutarMontajeDispararArma(bApuntando);
			Escopeta->DispararEscopeta(Objetivos);
			EstadoCombate = EEstadosCombate::EEC_Desocupado;
		}
	}
}

void UCombateComponente::EmpezarTemporizadorDisparo() {
	if(!DispareitorPersonaje || !ArmaEquipada) {
		return;
	}

	DispareitorPersonaje->GetWorldTimerManager().SetTimer(TemporizadorDisparo, this, &UCombateComponente::TerminadoDisparoTemporizador, ArmaEquipada->RetardoAlDisparar);
}

void UCombateComponente::TerminadoDisparoTemporizador() {
	if(!DispareitorPersonaje || !ArmaEquipada) {
		return;
	}

	bPuedoDisparar = true;
	if(bDispararPresionado && ArmaEquipada->bAutomatica) {
		Disparar();
	}

	RecargarArmaVacia(); // Recargar automaticamente cuando se queda sin municion en el arma 
}

bool UCombateComponente::PuedoDisparar() {
	if(!ArmaEquipada || !DispareitorPersonaje->bIntercambiarArmasFinalizado) {
		return false;
	}

	if(!ArmaEquipada->EstaSinMunicion() && bPuedoDisparar && EstadoCombate == EEstadosCombate::EEC_Recargando && ArmaEquipada->ObtenerTipoArma() == ETipoArma::ETA_Escopeta) {
		return true;
	}

	if(bRecargandoLocalmente) {
		return false;
	}

	return !ArmaEquipada->EstaSinMunicion() && bPuedoDisparar && EstadoCombate == EEstadosCombate::EEC_Desocupado;
}

// Llamado por ADispareitorPersonaje::ArrojarGranadaPulsado
void UCombateComponente::ArrojarGranada() {
	if(GranadasActuales == 0 || EstadoCombate != EEstadosCombate::EEC_Desocupado || !ArmaEquipada) {
		return;
	}

	EstadoCombate = EEstadosCombate::EEC_LanzandoGranada;
	if(DispareitorPersonaje) {
		DispareitorPersonaje->EjecutarMontajeArrojarGranada();
		UnirActorAManoIzquierda(ArmaEquipada);
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
		UnirActorAManoIzquierda(ArmaEquipada);
		MostrarGranada(true);
	}

	GranadasActuales = FMath::Clamp(GranadasActuales - 1, 0, GranadasMaximo);
	ActualizarGranadasHUD();
} 

void UCombateComponente::AlReplicar_GranadasActuales() {
	ActualizarGranadasHUD();
}

void UCombateComponente::ActualizarGranadasHUD() {
	DispareitorControladorJugador = DispareitorControladorJugador ? DispareitorControladorJugador : Cast<ADispareitorControladorJugador>(DispareitorPersonaje->Controller); 
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
	UnirActorAManoDerecha(ArmaEquipada);
}

void UCombateComponente::RecibidaNotificacionAnimacion_IntercambiarArmas() {
	if(DispareitorPersonaje) {
		if(DispareitorPersonaje->HasAuthority()) {
			EstadoCombate = EEstadosCombate::EEC_Desocupado;
		}
	}
}

void UCombateComponente::RecibidaNotificacionAnimacion_IntercambiarArmasFinalizado() {
	ArmaEquipada->ActualizarEstado(EEstado::EEA_Equipada); 
	UnirActorAManoDerecha(ArmaEquipada);
	ArmaEquipada->ActualizarMunicionHUD();
	ActualizarMunicionPersonaje();
	EjecutarSonidoAlEquipar(ArmaEquipada);
	RecargarArmaVacia();

	ArmaSecundariaEquipada->ActualizarEstado(EEstado::EEA_EquipadaSecundaria); 
	UnirActorAMochila(ArmaSecundariaEquipada);	

	DispareitorPersonaje->bIntercambiarArmasFinalizado = true;
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

bool UCombateComponente::PuedoIntercambiarArmas() {
	return ArmaEquipada && ArmaSecundariaEquipada && EstadoCombate == EEstadosCombate::EEC_Desocupado;
}

void UCombateComponente::AlReplicar_SosteniendoBandera() {
	if(bSosteniendoBandera && DispareitorPersonaje && DispareitorPersonaje->IsLocallyControlled()) {
		DispareitorPersonaje->Crouch();
	}
}