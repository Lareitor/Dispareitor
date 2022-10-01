#include "DispareitorPersonaje.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/WidgetComponent.h"
#include "Net/UnrealNetwork.h"
#include "Dispareitor/Arma/Arma.h"
#include "Dispareitor/DispareitorComponentes/CombateComponente.h"
#include "Dispareitor/DispareitorComponentes/BuffComponente.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "DispareitorInstanciaAnimacion.h"
#include "Dispareitor/Dispareitor.h"
#include "Dispareitor/ControladorJugador/DispareitorControladorJugador.h"
#include "Dispareitor/ModoJuego/DispareitorModoJuego.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Particles/ParticleSystemComponent.h"
#include "Dispareitor/EstadoJugador/DispareitorEstadoJugador.h"
#include "Dispareitor/Tipos/TiposArma.h"
#include "Dispareitor/HUD/HUDSobreLaCabeza.h"
#include "Components/BoxComponent.h"
#include "Dispareitor/DispareitorComponentes/CompensacionLagComponente.h"

ADispareitorPersonaje::ADispareitorPersonaje() {
	PrimaryActorTick.bCanEverTick = true;

	SpawnCollisionHandlingMethod = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	BrazoCamara = CreateDefaultSubobject<USpringArmComponent>(TEXT("BrazoCamara"));
	BrazoCamara->SetupAttachment(GetMesh());
	BrazoCamara->TargetArmLength = 600.f;
	BrazoCamara->bUsePawnControlRotation = true; 

	Camara = CreateDefaultSubobject<UCameraComponent>(TEXT("Camara"));
	Camara->SetupAttachment(BrazoCamara, USpringArmComponent::SocketName);
	Camara->bUsePawnControlRotation = false;

	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true; 

	HUDSobreLaCabeza = CreateDefaultSubobject<UWidgetComponent>(TEXT("HUDSobreLaCabeza"));
	HUDSobreLaCabeza->SetupAttachment(RootComponent);

	CombateComponente = CreateDefaultSubobject<UCombateComponente>(TEXT("CombateComponente"));
	// No necesitamos registrar los componentes en GetLifetimeReplicatedProps, solo necesitamos activarles esta propiedad
	CombateComponente->SetIsReplicated(true);

	BuffComponente = CreateDefaultSubobject<UBuffComponente>(TEXT("BuffComponente"));
	BuffComponente->SetIsReplicated(true);

	CompensacionLagComponente = CreateDefaultSubobject<UCompensacionLagComponente>(TEXT("CompensacionLagComponente"));

	// Activar que se pueda agachar (esta propiedad tambien existe en el BP)
	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;

	//Para evitar que un jugador choque con la camara de otro
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore); 
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore); 
	// Le asignamos a la malla el nuevo tipo de canal que hemos creado para tener mayor precision al disparar
	GetMesh()->SetCollisionObjectType(ECC_MallaDelEsqueleto);
	// Para que el rayo que lanzamos desde la cruceta impacte a los jugadores y poder asi cambiar su color a rojo
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block); 

	GetCharacterMovement()->RotationRate = FRotator(0.f, 0.f, 850.f);

	GirarEnSitio = EGirarEnSitio::EGES_NoGirar;
	NetUpdateFrequency = 66.f;
	MinNetUpdateFrequency = 33.f;

	ComponenteLineaDelTiempoParaDisolucion = CreateDefaultSubobject<UTimelineComponent>(TEXT("ComponenteLineaDelTiempoParaDisolucion"));

	Granada = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Granada"));
	Granada->SetupAttachment(GetMesh(), FName("GranadaSocket"));
	Granada->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	CajaCabeza = CreateDefaultSubobject<UBoxComponent>(TEXT("CajaCabeza"));
	CajaCabeza->SetupAttachment(GetMesh(), FName("head"));
	CajasColision.Add(FName("head"), CajaCabeza);

	CajaPelvis = CreateDefaultSubobject<UBoxComponent>(TEXT("CajaPelvis"));
	CajaPelvis->SetupAttachment(GetMesh(), FName("pelvis"));
	CajasColision.Add(FName("pelvis"), CajaPelvis);
	
	CajaEspina02 = CreateDefaultSubobject<UBoxComponent>(TEXT("CajaEspina02"));
	CajaEspina02->SetupAttachment(GetMesh(), FName("spine_02"));
	CajasColision.Add(FName("spine_02"), CajaEspina02);

	CajaEspina03 = CreateDefaultSubobject<UBoxComponent>(TEXT("CajaEspina03"));
	CajaEspina03->SetupAttachment(GetMesh(), FName("spine_03"));
	CajasColision.Add(FName("spine_03"), CajaEspina03);

	CajaBrazoI = CreateDefaultSubobject<UBoxComponent>(TEXT("CajaBrazoI"));
	CajaBrazoI->SetupAttachment(GetMesh(), FName("upperarm_l"));
	CajasColision.Add(FName("upperarm_l"), CajaBrazoI);
	
	CajaBrazoD = CreateDefaultSubobject<UBoxComponent>(TEXT("CajaBrazoD"));
	CajaBrazoD->SetupAttachment(GetMesh(), FName("upperarm_r"));
	CajasColision.Add(FName("upperarm_r"), CajaBrazoD);
	
	CajaAntebrazoI = CreateDefaultSubobject<UBoxComponent>(TEXT("CajaAntebrazoI"));
	CajaAntebrazoI->SetupAttachment(GetMesh(), FName("lowerarm_l"));
	CajasColision.Add(FName("lowerarm_l"), CajaAntebrazoI);
	
	CajaAntebrazoD = CreateDefaultSubobject<UBoxComponent>(TEXT("CajaAntebrazoD"));
	CajaAntebrazoD->SetupAttachment(GetMesh(), FName("lowerarm_r"));
	CajasColision.Add(FName("lowerarm_r"), CajaAntebrazoD);
	
	CajaManoI = CreateDefaultSubobject<UBoxComponent>(TEXT("CajaManoI"));
	CajaManoI->SetupAttachment(GetMesh(), FName("hand_l"));
	CajasColision.Add(FName("hand_l"), CajaManoI);
	
	CajaManoD = CreateDefaultSubobject<UBoxComponent>(TEXT("CajaManoD"));
	CajaManoD->SetupAttachment(GetMesh(), FName("hand_r"));
	CajasColision.Add(FName("hand_r"), CajaManoD);
	
	CajaMochila = CreateDefaultSubobject<UBoxComponent>(TEXT("CajaMochila"));
	CajaMochila->SetupAttachment(GetMesh(), FName("backpack"));
	CajasColision.Add(FName("backpack"), CajaMochila);
	
	CajaManta = CreateDefaultSubobject<UBoxComponent>(TEXT("CajaManta"));
	CajaManta->SetupAttachment(GetMesh(), FName("blanket_l"));
	CajasColision.Add(FName("blanket_l"), CajaManta);
	
	CajaMusloI = CreateDefaultSubobject<UBoxComponent>(TEXT("CajaMusloI"));
	CajaMusloI->SetupAttachment(GetMesh(), FName("thigh_l"));
	CajasColision.Add(FName("thigh_l"), CajaMusloI);
	
	CajaMusloD = CreateDefaultSubobject<UBoxComponent>(TEXT("CajaMusloD"));
	CajaMusloD->SetupAttachment(GetMesh(), FName("thigh_r"));
	CajasColision.Add(FName("thigh_r"), CajaMusloD);
	
	CajaGemeloI = CreateDefaultSubobject<UBoxComponent>(TEXT("CajaGemeloI"));
	CajaGemeloI->SetupAttachment(GetMesh(), FName("calf_l"));
	CajasColision.Add(FName("calf_l"), CajaGemeloI);

	CajaGemeloD = CreateDefaultSubobject<UBoxComponent>(TEXT("CajaGemeloD"));
	CajaGemeloD->SetupAttachment(GetMesh(), FName("calf_r"));
	CajasColision.Add(FName("calf_r"), CajaGemeloD);

	CajaPieI = CreateDefaultSubobject<UBoxComponent>(TEXT("CajaPieI"));
	CajaPieI->SetupAttachment(GetMesh(), FName("foot_l"));
	CajasColision.Add(FName("foot_l"), CajaPieI);

	CajaPieD = CreateDefaultSubobject<UBoxComponent>(TEXT("CajaPieD"));
	CajaPieD->SetupAttachment(GetMesh(), FName("foot_r"));
	CajasColision.Add(FName("foot_r"), CajaPieD);

	for(auto CajaColision : CajasColision) {
		if(CajaColision.Value) {
			CajaColision.Value->SetCollisionObjectType(ECC_CajaColision);
			CajaColision.Value->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
			CajaColision.Value->SetCollisionResponseToChannel(ECC_CajaColision, ECollisionResponse::ECR_Block);
			CajaColision.Value->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		}
	}
}

// En esta funcion es donde registramos las variables que queremos replicar
void ADispareitorPersonaje::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Solo se replica el ArmaSolapada en el cliente que posee ADispareitorPersonaje
	DOREPLIFETIME_CONDITION(ADispareitorPersonaje, ArmaSolapada, COND_OwnerOnly);
	DOREPLIFETIME(ADispareitorPersonaje, Vida);
	DOREPLIFETIME(ADispareitorPersonaje, Escudo);
	DOREPLIFETIME(ADispareitorPersonaje, bSoloGirarCamara);
}

void ADispareitorPersonaje::BeginPlay() {
	Super::BeginPlay();	

	if(HasAuthority()) { // Somos el servidor. Es lo mismo que GetLocalRole() == ENetRole::ROLE_Authority
		// Enlazamos nuestro metodo de recibir daño al delegado, para que se invoque cuando ProyectilBala llame a ApplyDamage
		OnTakeAnyDamage.AddDynamic(this, &ADispareitorPersonaje::RecibirDanio);
	}
	if(Granada) {
		Granada->SetVisibility(false);
	}
}

void ADispareitorPersonaje::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	CalcularRotarEnSitio(DeltaTime);
	EsconderCamaraSiPersonajeCerca();
	SondearInicializacion();
}

void ADispareitorPersonaje::CalcularRotarEnSitio(float DeltaTime) {
	if(bSoloGirarCamara) {
		bUseControllerRotationYaw = false;
		GirarEnSitio = EGirarEnSitio::EGES_NoGirar;	
		return;
	}

	// ROLE_SimulatedProxy = 1, ROLE_AutonomousProxy = 2, ROLE_Authority = 3
	// Solo lo ejecutamos para las instancias que tiene control local o están en el server
	if(GetLocalRole() > ENetRole::ROLE_SimulatedProxy && IsLocallyControlled()) {
		CalcularGiroEInclinacionParadoYArmado(DeltaTime);
	} else {
		TiempoDesdeUltimaReplicacionDeMovimiento += DeltaTime;
		if(TiempoDesdeUltimaReplicacionDeMovimiento > 0.25f) {
			OnRep_ReplicatedMovement();
		}
		CalcularInclinacion();
	}
}


void ADispareitorPersonaje::SondearInicializacion() {
	if(DispareitorEstadoJugador == nullptr) {
		// En los primeros frames este casting siempre devolverá null 
		DispareitorEstadoJugador = GetPlayerState<ADispareitorEstadoJugador>();
		if(DispareitorEstadoJugador) {
			DispareitorEstadoJugador->IncrementarMuertos(0.f);	
			DispareitorEstadoJugador->IncrementarMuertes(0);
		}
	}

	if(DispareitorControladorJugador == nullptr) {
		DispareitorControladorJugador = Cast<ADispareitorControladorJugador>(Controller);
		if(DispareitorControladorJugador) {
			ReaparecerArmaPorDefecto();
			ActualizarMunicionHUD();
			ActualizarVidaHUD();
			ActualizarEscudoHUD();
			ActualizarGranadasHUD();
		} 
	}	

	/*if(HUDSobreLaCabeza) {
		HUDSobreLaCabezaReal = HUDSobreLaCabezaReal != nullptr ? HUDSobreLaCabezaReal : Cast<UHUDSobreLaCabeza>(HUDSobreLaCabeza->GetUserWidgetObject());
		HUDSobreLaCabezaReal->MostrarJugadorNombre(this);
	}*/
}

// Cada vez que el personaje se mueve llama a esta funcion, asi que la podemos usar en lugar del tick para calcular el giro en los proxies simulados
// El problema es que solo se llama cuando se mueve, pero nos interesa llamarla regularmente, por lo que utilizamos la variable TiempoDesdeUltimaReplicacionDeMovimiento
// para almacenar el tiempo desde la ultima replicacion de movimiento y si pasa un cierto umbral volvemos a llamar a esta funcion 
void ADispareitorPersonaje::OnRep_ReplicatedMovement() {
	Super::OnRep_ReplicatedMovement();
	CalcularGiroParadoYArmadoEnProxiesSimulados();	
	TiempoDesdeUltimaReplicacionDeMovimiento = 0.f;
}

void ADispareitorPersonaje::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) {
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("Saltar", IE_Pressed, this, &ADispareitorPersonaje::Jump);
	PlayerInputComponent->BindAction("EquiparPulsado", IE_Pressed, this, &ADispareitorPersonaje::EquiparPulsado);
	PlayerInputComponent->BindAction("AgacharPulsado", IE_Pressed, this, &ADispareitorPersonaje::AgacharPulsado);
	PlayerInputComponent->BindAction("Apuntar", IE_Pressed, this, &ADispareitorPersonaje::ApuntarPulsado);
	PlayerInputComponent->BindAction("Apuntar", IE_Released, this, &ADispareitorPersonaje::ApuntarLiberado);
	PlayerInputComponent->BindAction("Disparar", IE_Pressed, this, &ADispareitorPersonaje::DispararPulsado);
	PlayerInputComponent->BindAction("Disparar", IE_Released, this, &ADispareitorPersonaje::DispararLiberado);
	PlayerInputComponent->BindAction("RecargarPulsado", IE_Pressed, this, &ADispareitorPersonaje::RecargarPulsado);
	PlayerInputComponent->BindAction("ArrojarGranadaPulsado", IE_Pressed, this, &ADispareitorPersonaje::ArrojarGranadaPulsado);

	PlayerInputComponent->BindAxis("MoverAdelanteAtrasPulsado", this, &ADispareitorPersonaje::MoverAdelanteAtrasPulsado);
	PlayerInputComponent->BindAxis("MoverIzquierdaDerechaPulsado", this, &ADispareitorPersonaje::MoverIzquierdaDerechaPulsado);
	PlayerInputComponent->BindAxis("GirarIzquierdaDerechaPulsado", this, &ADispareitorPersonaje::GirarIzquierdaDerechaPulsado);
	PlayerInputComponent->BindAxis("GirarArribaAbajoPulsado", this, &ADispareitorPersonaje::GirarArribaAbajoPulsado);
}

void ADispareitorPersonaje::PostInitializeComponents() {
	Super::PostInitializeComponents();
	if(CombateComponente) {
		CombateComponente->DispareitorPersonaje = this;
	}
	if(BuffComponente) {
		BuffComponente->DispareitorPersonaje = this;
		BuffComponente->InicializarVelocidadesOriginales(GetCharacterMovement()->MaxWalkSpeed, GetCharacterMovement()->MaxWalkSpeedCrouched);
		BuffComponente->InicializarSaltoOriginal(GetCharacterMovement()->JumpZVelocity);
	}
	if(CompensacionLagComponente) {
		CompensacionLagComponente->DispareitorPersonaje = this;
		if(Controller) {
			CompensacionLagComponente->DispareitorControladorJugador = Cast<ADispareitorControladorJugador>(Controller);
		}
	}
}

void ADispareitorPersonaje::MoverAdelanteAtrasPulsado(float Valor) {
	if(bSoloGirarCamara) {
		return;
	}

	if(Controller != nullptr && Valor != 0.f) {
		const FRotator RotacionGiro(0.f, Controller->GetControlRotation().Yaw, 0.f);
		const FVector DireccionAdelanteDeRotacionGiro(FRotationMatrix(RotacionGiro).GetUnitAxis(EAxis::X));
		AddMovementInput(DireccionAdelanteDeRotacionGiro, Valor);
	}
}

void ADispareitorPersonaje::MoverIzquierdaDerechaPulsado(float Valor) {
	if(bSoloGirarCamara) {
		return;
	}

	if(Controller != nullptr && Valor != 0.f) {
		const FRotator RotacionGiro(0.f, Controller->GetControlRotation().Yaw, 0.f);
		const FVector DireccionLateralDeRotacionGiro(FRotationMatrix(RotacionGiro).GetUnitAxis(EAxis::Y));
		AddMovementInput(DireccionLateralDeRotacionGiro, Valor);
	}
}

void ADispareitorPersonaje::GirarIzquierdaDerechaPulsado(float Valor) {
	AddControllerYawInput(Valor);
}

void ADispareitorPersonaje::GirarArribaAbajoPulsado(float Valor) {
	AddControllerPitchInput(Valor);
}

void ADispareitorPersonaje::AgacharPulsado() {
	if(bSoloGirarCamara) {
		return;
	}

	if(bIsCrouched) {
		UnCrouch();
	} else {
		Crouch(); // Si se lleva a cabo con exito activa Character.bIsCrouched a 1 (true) (ademas de replicarlo automaticamente en los clientes) que podemos chequear en nuestro clase animacion
	}
}

void ADispareitorPersonaje::Jump() {
	if(bSoloGirarCamara) {
		return;
	}

	if(bIsCrouched) {
		UnCrouch();
	} else {
		Super::Jump();
	}
}

void ADispareitorPersonaje::ApuntarPulsado() {
	if(bSoloGirarCamara) {
		return;
	}

	if(CombateComponente) {
		CombateComponente->ActualizarApuntando(true);
	}
}

void ADispareitorPersonaje::ApuntarLiberado() {
	if(bSoloGirarCamara) {
		return;
	}

	if(CombateComponente) {
		CombateComponente->ActualizarApuntando(false);
	}
}

void ADispareitorPersonaje::DispararPulsado() {
	if(bSoloGirarCamara) {
		return;
	}

	if(CombateComponente) {
		CombateComponente->DispararPresionado(true);
	}
}

void ADispareitorPersonaje::DispararLiberado() {
	if(bSoloGirarCamara) {
		return;
	}

	if(CombateComponente) {
		CombateComponente->DispararPresionado(false);
	}
}

void ADispareitorPersonaje::ArrojarGranadaPulsado() {
	if(CombateComponente) {
		CombateComponente->ArrojarGranada();
	}
}

void ADispareitorPersonaje::RecargarPulsado() {
	if(bSoloGirarCamara) {
		return;
	}

	if(CombateComponente) {
		CombateComponente->Recargar();
	}
}

void ADispareitorPersonaje::EquiparPulsado() {
	if(bSoloGirarCamara) {
		return;
	}

	if(CombateComponente) {
		if(CombateComponente->EstadoCombate == EEstadosCombate::EEC_Desocupado) {
			Equipar_EnServidor();	
		}
		if(CombateComponente->PuedoIntercambiarArmas() && !HasAuthority() && CombateComponente->EstadoCombate == EEstadosCombate::EEC_Desocupado && !ArmaSolapada) {
			EjecutarMontajeIntercambiarArmas();
			CombateComponente->EstadoCombate = EEstadosCombate::EEC_IntercambiandoArmas;
			bIntercambiarArmasFinalizado = false;
		}	
	}
}

// Aunque la definicion de la funcion es Equipar_EnServidor hay que añadirle _Implementation, ya que UE creará Equipar_EnServidor y nosotros _Implementation que incluirá el codigo que se ejecuta en el servidor  
void ADispareitorPersonaje::Equipar_EnServidor_Implementation() {
	if(CombateComponente) {
		if(ArmaSolapada) {
			CombateComponente->EquiparArma(ArmaSolapada);
		} else if(CombateComponente->PuedoIntercambiarArmas()) {
			CombateComponente->IntercambiarArmas();
		}
	}
}

// En el servidor (Para el caso en el que el jugador sea ademas el servidor)
// Sabemos que esta funcion solo es llamada en el servidor 
// Llamada por AArma::Callback_EsferaSolapadaInicio y AArma::Callback_EsferaSolapadaFin
void ADispareitorPersonaje::ActivarArmaSolapada(AArma* Arma) {
	if(IsLocallyControlled()) { 
		if(ArmaSolapada) {
			ArmaSolapada->MostrarLeyendaSobreArma(false);
		}
	}

	ArmaSolapada = Arma;

	if(IsLocallyControlled()) { // y ademas somos el jugador que está hospedando el juego
		if(ArmaSolapada) { 
			ArmaSolapada->MostrarLeyendaSobreArma(true);
		}
	}
}

// En los clientes (Para el caso en el que el jugador no sea el servidor)
// Se llama automaticamente en el cliente (hemos especificado arriba que ArmaSolapada solo replique su estado en el cliente que posee ADispareitorPersonaje) cuando la variable es replicada por el servidor. Nunca se llama en el servidor
// Acepta 0 ó 1 argumento. Si le pasamos argumento tiene que ser del tipo de la variable replicada, y se rellenará con el valor anterior replicado o null si no lo tuviera
void ADispareitorPersonaje::ArmaSolapada_AlReplicar(AArma* ArmaReplicadaAnterior) {
	if(ArmaSolapada) {
		ArmaSolapada->MostrarLeyendaSobreArma(true);
	}
	if(ArmaReplicadaAnterior) {
		ArmaReplicadaAnterior->MostrarLeyendaSobreArma(false);
	}
}

// Llamado por Tick
// Calcular el desplazamiento del giro (yaw) e inclinacion (pitch) cuando estamos parados y armados
void ADispareitorPersonaje::CalcularGiroEInclinacionParadoYArmado(float DeltaTime) {
	if(CombateComponente && CombateComponente->ArmaEquipada == nullptr) {
		return;
	}

	bUseControllerRotationYaw = true;
    float Velocidad = CalcularVelocidad();
	bool bEnElAire = GetCharacterMovement()->IsFalling();

	if(Velocidad == 0.f && !bEnElAire) { // Está parado y no saltando
		bRotarHuesoRaiz = true;
		FRotator ArmadoRotacionActual = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		FRotator DeltaArmadoRotacion = UKismetMathLibrary::NormalizedDeltaRotator(ArmadoRotacionActual, ArmadoRotacionInicial);
		AOGiro = DeltaArmadoRotacion.Yaw;
		if(GirarEnSitio == EGirarEnSitio::EGES_NoGirar) {
			AOGiroInterpolacion = AOGiro;
		}
		CalcularGirarEnSitio(DeltaTime);
	} else { // corriendo o saltando (Velocidad > 0.f || bEnElAire)
		bRotarHuesoRaiz = false;
		AOGiro = 0.f;
		ArmadoRotacionInicial = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		GirarEnSitio = EGirarEnSitio::EGES_NoGirar;
	}

	CalcularInclinacion();
}

void ADispareitorPersonaje::CalcularGirarEnSitio(float DeltaTime) {
	if(AOGiro > 90.f) {
		GirarEnSitio = EGirarEnSitio::EGES_Derecha;
	} else if(AOGiro < -90.f ) {
		GirarEnSitio = EGirarEnSitio::EGES_Izquierda;
	}

	if(GirarEnSitio != EGirarEnSitio::EGES_NoGirar) {
		AOGiroInterpolacion = FMath::FInterpTo(AOGiroInterpolacion, 0.f, DeltaTime, 4.f);
		AOGiro = AOGiroInterpolacion;
		if(FMath::Abs(AOGiro) < 15.f) {
			GirarEnSitio = EGirarEnSitio::EGES_NoGirar;
			ArmadoRotacionInicial = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		}
	}
}

// Llamad por Tick y CalcularGiroEInclinacionParadoYArmado
void ADispareitorPersonaje::CalcularInclinacion() {
	AOInclinacion = GetBaseAimRotation().Pitch;
	// Debido a la compresion que realiza UE a la hora de enviar estos valores por la red, transforma los valores negativos en positivos, por los que cuando estamos mirando hacia abajo en el cliente 
	// (inclinacion negativa) en el server volverá a mirar hacia arriba
	// Para solucionarlo utilizamos esta solucion  
	if(AOInclinacion > 90.f && !IsLocallyControlled()) { // estamos en el server
		// Mapear Inclinación [270,360) a [-90, 0)]
		FVector2D RangoEntrada(270.f, 360.f);
		FVector2D RangoSalida(-90.f, 0.f);
		AOInclinacion = FMath::GetMappedRangeValueClamped(RangoEntrada, RangoSalida, AOInclinacion);
	}
}

// Llamado solo por los proxies simulados para implementar el giro y evitar de este modo el jittering  
void ADispareitorPersonaje::CalcularGiroParadoYArmadoEnProxiesSimulados() {
	if(CombateComponente == nullptr || CombateComponente->ArmaEquipada == nullptr) {
		return;
	}

	bRotarHuesoRaiz = false;

	float Velocidad = CalcularVelocidad();
	if(Velocidad > 0.f) {
		GirarEnSitio = EGirarEnSitio::EGES_NoGirar;		
		return;
	}

	RotacionFrameAnteriorEnProxy = RotacionFrameActualEnProxy;
	RotacionFrameActualEnProxy = GetActorRotation();
	GiroEnProxy = UKismetMathLibrary::NormalizedDeltaRotator(RotacionFrameActualEnProxy, RotacionFrameAnteriorEnProxy).Yaw;

	if(FMath::Abs(GiroEnProxy) > UmbralDeGiroEnProxy) {
		if(GiroEnProxy > UmbralDeGiroEnProxy) {
			GirarEnSitio = 	EGirarEnSitio::EGES_Derecha;
		} else if(GiroEnProxy < -UmbralDeGiroEnProxy) {
			GirarEnSitio = EGirarEnSitio::EGES_Izquierda;
		} else {
			GirarEnSitio = EGirarEnSitio::EGES_NoGirar;	
		}
	} else {
		GirarEnSitio = EGirarEnSitio::EGES_NoGirar;		
	}
}

AArma* ADispareitorPersonaje::ObtenerArmaEquipada() {
	return CombateComponente != nullptr ? CombateComponente->ArmaEquipada : nullptr;
}

// Llamado por CombateComponente
void ADispareitorPersonaje::EjecutarMontajeDispararArma(bool bApuntando) {
	if(CombateComponente == nullptr || CombateComponente->ArmaEquipada == nullptr) {
		return;
	}

	UAnimInstance* InstanciaAnimacion = GetMesh()->GetAnimInstance();
	if(InstanciaAnimacion && MontajeDispararArma) {
		InstanciaAnimacion->Montage_Play(MontajeDispararArma);
		FName NombreSeccion = bApuntando ? FName("RifleMira") : FName("RifleCadera");
		InstanciaAnimacion->Montage_JumpToSection(NombreSeccion);
	}
}

// Llamado por UCombateComponente::RecargarServidor_Implementation
void ADispareitorPersonaje::EjecutarMontajeRecargar() {
	if(CombateComponente == nullptr || CombateComponente->ArmaEquipada == nullptr) {
		return;
	}

	UAnimInstance* InstanciaAnimacion = GetMesh()->GetAnimInstance();
	if(InstanciaAnimacion && MontajeRecargar) {
		InstanciaAnimacion->Montage_Play(MontajeRecargar);
		FName NombreSeccion;
		switch(CombateComponente->ArmaEquipada->ObtenerTipoArma()) {
			case ETipoArma::ETA_RifleAsalto:
				NombreSeccion = FName("RifleAsalto");	
				break;
			case ETipoArma::ETA_LanzaCohetes:
				NombreSeccion = FName("LanzaCohetes");	
				break;	
			case ETipoArma::ETA_Pistola:
				NombreSeccion = FName("Pistola");	
				break;	
			case ETipoArma::ETA_Subfusil:
				NombreSeccion = FName("Pistola");	
				break;	
			case ETipoArma::ETA_Escopeta:
				NombreSeccion = FName("Escopeta");	
				break;		
			case ETipoArma::ETA_Francotirador:
				NombreSeccion = FName("Francotirador");	
				break;	
			case ETipoArma::ETA_LanzaGranadas:
				NombreSeccion = FName("LanzaGranadas");	
				break;	
		}
		InstanciaAnimacion->Montage_JumpToSection(NombreSeccion);
	}
}


// Llamado por RecibirDanio y AlReplicar_Vida
void ADispareitorPersonaje::EjecutarMontajeReaccionAImpacto() {
	if(CombateComponente == nullptr || CombateComponente->ArmaEquipada == nullptr) {
		return;
	}

	UAnimInstance* InstanciaAnimacion = GetMesh()->GetAnimInstance();
	if(InstanciaAnimacion && MontajeReaccionAImpacto) {
		InstanciaAnimacion->Montage_Play(MontajeReaccionAImpacto);
		FName NombreSeccion("DesdeAdelante") ;
		InstanciaAnimacion->Montage_JumpToSection(NombreSeccion);
	}
}

// Llamado por Eliminado
void ADispareitorPersonaje::EjecutarMontajeEliminado() {
	UAnimInstance* InstanciaAnimacion = GetMesh()->GetAnimInstance();
	if(InstanciaAnimacion && MontajeEliminado) {
		InstanciaAnimacion->Montage_Play(MontajeEliminado);
	}
}

void ADispareitorPersonaje::EjecutarMontajeArrojarGranada() {
	UAnimInstance* InstanciaAnimacion = GetMesh()->GetAnimInstance();
	if(InstanciaAnimacion && MontajeArrojarGranada) {
		InstanciaAnimacion->Montage_Play(MontajeArrojarGranada);
	}
}

void ADispareitorPersonaje::EjecutarMontajeIntercambiarArmas() {
	UAnimInstance* InstanciaAnimacion = GetMesh()->GetAnimInstance();
	if(InstanciaAnimacion && MontajeIntercambiarArmas) {
		InstanciaAnimacion->Montage_Play(MontajeIntercambiarArmas);
	}	
}

FVector ADispareitorPersonaje::ObtenerObjetoAlcanzado() const {
	return (CombateComponente != nullptr && CombateComponente->ArmaEquipada != nullptr) ? CombateComponente->ObjetoAlcanzado : FVector();	
}

// Llamado por Tick
void ADispareitorPersonaje::EsconderCamaraSiPersonajeCerca() {
	if(!IsLocallyControlled()) {
		return;
	}
	if((Camara->GetComponentLocation() - GetActorLocation()).Size() < DistanciaMinimaEntrePersonajeYCamara) {
		GetMesh()->SetVisibility(false);
		if(CombateComponente && CombateComponente->ArmaEquipada && CombateComponente->ArmaEquipada->ObtenerMalla()) {
			CombateComponente->ArmaEquipada->ObtenerMalla()->bOwnerNoSee = true;
		}
	} else {
		GetMesh()->SetVisibility(true);
		if(CombateComponente && CombateComponente->ArmaEquipada && CombateComponente->ArmaEquipada->ObtenerMalla()) {
			CombateComponente->ArmaEquipada->ObtenerMalla()->bOwnerNoSee = false;
		}
	}
}

// LLamado al recibir daño por parte de AProyectilBala::Callback_AlImpactar y ALimitesJuego::Callback_SolapadaMallaInicio
// Solo se ejecuta en el server
void ADispareitorPersonaje::RecibirDanio(AActor* ActorDaniado, float Danio, const UDamageType* TipoDanio, class AController* ControladorInstigador, AActor* ActorCausante) {
	if(bEliminado) {
		return;
	}

	float DanioAVida = Danio;
	if(Escudo > 0.f) {
		if(Escudo >= Danio) {
			Escudo = FMath::Clamp(Escudo - Danio, 0.f, EscudoMaximo);
			DanioAVida = 0.f;
		} else {
			DanioAVida = FMath::Clamp(Danio - Escudo, 0.f, Danio);
			Escudo = 0.f;
		}
	}

	Vida = FMath::Clamp(Vida - DanioAVida, 0.f, VidaMaxima);
	ActualizarVidaHUD();
	ActualizarEscudoHUD();
	EjecutarMontajeReaccionAImpacto();

	if(Vida == 0.f) {
		ADispareitorModoJuego* DispareitorModoJuego = GetWorld()->GetAuthGameMode<ADispareitorModoJuego>();
		if(DispareitorModoJuego) {
			DispareitorControladorJugador = DispareitorControladorJugador != nullptr ? DispareitorControladorJugador : Cast<ADispareitorControladorJugador>(Controller);
			ADispareitorControladorJugador* AtacanteDispareitorControladorJugador = Cast<ADispareitorControladorJugador>(ControladorInstigador);
			DispareitorModoJuego->JugadorEliminado(this, DispareitorControladorJugador, AtacanteDispareitorControladorJugador);
		}
	}
}

// Se ejecuta en los clientes
void ADispareitorPersonaje::AlReplicar_Vida(float VidaAnterior) {
	ActualizarVidaHUD();
	if(Vida < VidaAnterior) { // Solo ejecutamos el montaje cuando nuestra vida se decrementa porque estamos recibiendo daño
		EjecutarMontajeReaccionAImpacto();
	}
}

void ADispareitorPersonaje::AlReplicar_Escudo(float EscudoAnterior) {
	ActualizarEscudoHUD();
	if(Escudo < EscudoAnterior) { // Solo ejecutamos el montaje cuando nuestra vida se decrementa porque estamos recibiendo daño
		EjecutarMontajeReaccionAImpacto();
	}
}

void ADispareitorPersonaje::ActualizarVidaHUD() {
	DispareitorControladorJugador = DispareitorControladorJugador != nullptr ? DispareitorControladorJugador : Cast<ADispareitorControladorJugador>(Controller);
	if(DispareitorControladorJugador) {
		DispareitorControladorJugador->ActualizarVidaHUD(Vida, VidaMaxima);
	}
}

void ADispareitorPersonaje::ActualizarEscudoHUD() {
	DispareitorControladorJugador = DispareitorControladorJugador != nullptr ? DispareitorControladorJugador : Cast<ADispareitorControladorJugador>(Controller);
	if(DispareitorControladorJugador) {
		DispareitorControladorJugador->ActualizarEscudoHUD(Escudo, EscudoMaximo);
	}
}

void ADispareitorPersonaje::ActualizarGranadasHUD() {
	DispareitorControladorJugador = DispareitorControladorJugador != nullptr ? DispareitorControladorJugador : Cast<ADispareitorControladorJugador>(Controller);
	if(DispareitorControladorJugador && CombateComponente) {
		DispareitorControladorJugador->ActualizarGranadasHUD(CombateComponente->ObtenerGranadasActuales());
	}
}

void ADispareitorPersonaje::ActualizarMunicionHUD() {
	DispareitorControladorJugador = DispareitorControladorJugador != nullptr ? DispareitorControladorJugador : Cast<ADispareitorControladorJugador>(Controller);
	if(DispareitorControladorJugador && CombateComponente && CombateComponente->ArmaEquipada) {
		DispareitorControladorJugador->ActualizarMunicionPersonajeHUD(CombateComponente->MunicionPersonaje);
		DispareitorControladorJugador->ActualizarMunicionArmaHUD(CombateComponente->ArmaEquipada->ObtenerMunicion());
	} 
}

// Llamado por DispareitorModoJuego::JugadorEliminado 
// Ejecutado en el server ya que DispareitorModoJuego solo existe en el server
void ADispareitorPersonaje::Eliminado() {
	SoltarODestruirArmas();
	Eliminado_Multicast();
	GetWorldTimerManager().SetTimer(TemporizadorEliminado, this, &ADispareitorPersonaje::TemporizadorEliminadoFinalizado, RetardoDeEliminacion);
}

void ADispareitorPersonaje::Eliminado_Multicast_Implementation() {
	bEliminado = true;
	EjecutarMontajeEliminado();

	if(InstanciaMaterialParaDisolucion) {
		InstanciaMaterialDinamicoParaDisolucion = UMaterialInstanceDynamic::Create(InstanciaMaterialParaDisolucion, this);
		GetMesh()->SetMaterial(0, InstanciaMaterialDinamicoParaDisolucion);
		InstanciaMaterialDinamicoParaDisolucion->SetScalarParameterValue(TEXT("Disolucion"), 0.55f);
		InstanciaMaterialDinamicoParaDisolucion->SetScalarParameterValue(TEXT("Brillo"), 200.f);
		EmpezarDisolucion();
	}

	GetCharacterMovement()->DisableMovement(); // Impide movimiento
	GetCharacterMovement()->StopMovementImmediately(); // Impide rotacion
	bSoloGirarCamara = true;
	if(CombateComponente) {
		CombateComponente->DispararPresionado(false);
	}
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	if(SistemaParticulasRobotEliminacion) {
		FVector RobotEliminacionPuntoAparicion(GetActorLocation().X, GetActorLocation().Y, GetActorLocation().Z + 200.f);
		ComponenteSistemaParticulasRobotEliminacion = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SistemaParticulasRobotEliminacion, RobotEliminacionPuntoAparicion, GetActorRotation());
	}
	if(SonidoRobotEliminacion) {
		UGameplayStatics::SpawnSoundAtLocation(this, SonidoRobotEliminacion, GetActorLocation());
	}
	if(HUDSobreLaCabeza) {
		HUDSobreLaCabeza->SetVisibility(false);
	}
	if(DispareitorControladorJugador) {
		DispareitorControladorJugador->ActualizarMunicionArmaHUD(0);
	}
}

void ADispareitorPersonaje::SoltarODestruirArmas() {
	if(CombateComponente) {
		if(CombateComponente->ArmaEquipada) {
			SoltarODestruirArma(CombateComponente->ArmaEquipada);
		}
		if(CombateComponente->ArmaSecundariaEquipada) {
			SoltarODestruirArma(CombateComponente->ArmaSecundariaEquipada);
		}
	}
}

void ADispareitorPersonaje::SoltarODestruirArma(AArma* Arma) {
	if(Arma == nullptr) {
		return;
	}

	if(Arma->bDestruirArma) { // Destruir el arma solo si es el arma por defecto
		Arma->Destroy();
	} else {
		Arma->Soltar();
	}
}

void ADispareitorPersonaje::EmpezarDisolucion() {
	DelegadoLineaDelTiempoParaDisolucion.BindDynamic(this, &ADispareitorPersonaje::Callback_ActualizarMaterialEnDisolucion);
	if(ComponenteLineaDelTiempoParaDisolucion && CurvaDeDisolucion) {
		ComponenteLineaDelTiempoParaDisolucion->AddInterpFloat(CurvaDeDisolucion, DelegadoLineaDelTiempoParaDisolucion);
		ComponenteLineaDelTiempoParaDisolucion->Play();
	}
}

void ADispareitorPersonaje::Callback_ActualizarMaterialEnDisolucion(float DisolucionValor) {
	if(InstanciaMaterialDinamicoParaDisolucion) {
		InstanciaMaterialDinamicoParaDisolucion->SetScalarParameterValue(TEXT("Disolucion"), DisolucionValor);
	}
}

void ADispareitorPersonaje::TemporizadorEliminadoFinalizado() {
	ADispareitorModoJuego* DispareitorModoJuego = GetWorld()->GetAuthGameMode<ADispareitorModoJuego>();
	if(DispareitorModoJuego) {
		DispareitorModoJuego->PeticionReaparecer(this, DispareitorControladorJugador);
	}
}

// Llamado por ADispareitorModoJuego::PeticionReaparecer
void ADispareitorPersonaje::Destroyed() {
	Super::Destroyed();

	if(ComponenteSistemaParticulasRobotEliminacion) {
		ComponenteSistemaParticulasRobotEliminacion->DestroyComponent();
	}

	ADispareitorModoJuego* DispareitorModoJuego = Cast<ADispareitorModoJuego>(UGameplayStatics::GetGameMode(this));
	if(CombateComponente && CombateComponente->ArmaEquipada && DispareitorModoJuego && DispareitorModoJuego->GetMatchState() != MatchState::InProgress) {
		CombateComponente->ArmaEquipada->Destroy();
	}
}

EEstadosCombate ADispareitorPersonaje::ObtenerEstadoCombate() const {
	return CombateComponente == nullptr ? EEstadosCombate::EEC_Maximo : CombateComponente->EstadoCombate;
}

bool ADispareitorPersonaje::HayArmaEquipada() {
	return CombateComponente && CombateComponente->ArmaEquipada;
}

bool ADispareitorPersonaje::EstaApuntando() {
	return CombateComponente && CombateComponente->bApuntando;
}

float ADispareitorPersonaje::CalcularVelocidad() {
	FVector VelocidadTemporal = GetVelocity();
    VelocidadTemporal.Z = 0.f;
    return VelocidadTemporal.Size();
}

void ADispareitorPersonaje::ReaparecerArmaPorDefecto() {
	ADispareitorModoJuego* DispareitorModoJuego = Cast<ADispareitorModoJuego>(UGameplayStatics::GetGameMode(this));
	UWorld* Mundo = GetWorld();
	if(DispareitorModoJuego && Mundo && !bEliminado && ClaseArmaPorDefecto) {
		AArma* ArmaPorDefecto = Mundo->SpawnActor<AArma>(ClaseArmaPorDefecto);
		ArmaPorDefecto->bDestruirArma = true;
		if(CombateComponente) {
			CombateComponente->EquiparArma(ArmaPorDefecto);
		}
	}
}

bool ADispareitorPersonaje::EstaRecargandoLocalmente() {
	return CombateComponente ? CombateComponente->bRecargandoLocalmente : false;
}