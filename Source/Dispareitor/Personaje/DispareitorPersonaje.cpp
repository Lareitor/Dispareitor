#include "DispareitorPersonaje.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/WidgetComponent.h"
#include "Net/UnrealNetwork.h"
#include "Dispareitor/Arma/Arma.h"
#include "Dispareitor/DispareitorComponentes/CombateComponente.h"
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

// TODO Hacer el respawn lo mas lejos de los jugadores  

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

	DisolucionLineaTiempoComponente = CreateDefaultSubobject<UTimelineComponent>(TEXT("DisolucionLineaTiempoComponente"));
}

// En esta funcion es donde registramos las variables que queremos replicar
void ADispareitorPersonaje::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Solo se replica el ArmaSolapada en el cliente que posee ADispareitorPersonaje
	DOREPLIFETIME_CONDITION(ADispareitorPersonaje, ArmaSolapada, COND_OwnerOnly);
	DOREPLIFETIME(ADispareitorPersonaje, Vida);
}

void ADispareitorPersonaje::BeginPlay() {
	Super::BeginPlay();	

	ActualizarHUDVida();
	if(HasAuthority()) {
		// Enlazamos nuestro metodo de recibir daño al delegado, para que se invoque cuando ProyectilBala llame a ApplyDamage
		OnTakeAnyDamage.AddDynamic(this, &ADispareitorPersonaje::RecibirDano);
	}
}

void ADispareitorPersonaje::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

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

	EsconderCamaraSiPersonajeCerca();
	SondearInicializacion();
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

	if(HUDSobreLaCabeza) {
		HUDSobreLaCabezaReal = HUDSobreLaCabezaReal != nullptr ? HUDSobreLaCabezaReal : Cast<UHUDSobreLaCabeza>(HUDSobreLaCabeza->GetUserWidgetObject());
		HUDSobreLaCabezaReal->MostrarJugadorNombre(this);
	}
}

// Cada vez que el personaje se mueve llama a esta funcion, asi que la podemos usar en lugar del tick para calcular el giro en los proxies simulados
// El problema es que solo se llama cuando se mueve, pero nos interesa llamarla regularmente, por lo que utilizamos la variable TiempoDesdeUltimaReplicacionDeMovimiento
// para almacenar el tiempo desde la ultima replicacion de movimiento y si pasa un cierto umbral volvemos a llamar a esta funcion 
void ADispareitorPersonaje::OnRep_ReplicatedMovement() {
	Super::OnRep_ReplicatedMovement();
	ProxiesSimuladosGiro();	
	TiempoDesdeUltimaReplicacionDeMovimiento = 0.f;
}

void ADispareitorPersonaje::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) {
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("Saltar", IE_Pressed, this, &ADispareitorPersonaje::Jump);
	PlayerInputComponent->BindAction("Equipar", IE_Pressed, this, &ADispareitorPersonaje::Equipar);
	PlayerInputComponent->BindAction("Agachar", IE_Pressed, this, &ADispareitorPersonaje::Agachar);
	PlayerInputComponent->BindAction("Apuntar", IE_Pressed, this, &ADispareitorPersonaje::ApuntarPulsado);
	PlayerInputComponent->BindAction("Apuntar", IE_Released, this, &ADispareitorPersonaje::ApuntarLiberado);
	PlayerInputComponent->BindAction("Disparar", IE_Pressed, this, &ADispareitorPersonaje::DispararPulsado);
	PlayerInputComponent->BindAction("Disparar", IE_Released, this, &ADispareitorPersonaje::DispararLiberado);
	PlayerInputComponent->BindAction("Recargar", IE_Pressed, this, &ADispareitorPersonaje::Recargar);


	PlayerInputComponent->BindAxis("MoverAdelanteAtras", this, &ADispareitorPersonaje::MoverAdelanteAtras);
	PlayerInputComponent->BindAxis("MoverIzquierdaDerecha", this, &ADispareitorPersonaje::MoverIzquierdaDerecha);
	PlayerInputComponent->BindAxis("Girar", this, &ADispareitorPersonaje::Girar);
	PlayerInputComponent->BindAxis("MirarArribaAbajo", this, &ADispareitorPersonaje::MirarArribaAbajo);
}

void ADispareitorPersonaje::PostInitializeComponents() {
	Super::PostInitializeComponents();
	if(CombateComponente) {
		CombateComponente->DispareitorPersonaje = this;
	}
}

void ADispareitorPersonaje::MoverAdelanteAtras(float Valor) {
	if(Controller != nullptr && Valor != 0.f) {
		const FRotator RotacionGiro(0.f, Controller->GetControlRotation().Yaw, 0.f);
		const FVector DireccionAdelanteDeRotacionGiro(FRotationMatrix(RotacionGiro).GetUnitAxis(EAxis::X));
		AddMovementInput(DireccionAdelanteDeRotacionGiro, Valor);
	}
}

void ADispareitorPersonaje::MoverIzquierdaDerecha(float Valor) {
	if(Controller != nullptr && Valor != 0.f) {
		const FRotator RotacionGiro(0.f, Controller->GetControlRotation().Yaw, 0.f);
		const FVector DireccionLateralDeRotacionGiro(FRotationMatrix(RotacionGiro).GetUnitAxis(EAxis::Y));
		AddMovementInput(DireccionLateralDeRotacionGiro, Valor);
	}
}

void ADispareitorPersonaje::Girar(float Valor) {
	AddControllerYawInput(Valor);
}

void ADispareitorPersonaje::MirarArribaAbajo(float Valor) {
	AddControllerPitchInput(Valor);
}

void ADispareitorPersonaje::Agachar() {
	if(bIsCrouched) {
		UnCrouch();
	} else {
		Crouch(); // Si se lleva a cabo con exito activa Character.bIsCrouched a 1 (true) (ademas de replicarlo automaticamente en los clientes) que podemos chequear en nuestro clase animacion
	}
}

void ADispareitorPersonaje::Jump() {
	if(bIsCrouched) {
		UnCrouch();
	} else {
		Super::Jump();
	}
}

void ADispareitorPersonaje::ApuntarPulsado() {
	if(CombateComponente) {
		CombateComponente->ActualizarApuntando(true);
	}
}

void ADispareitorPersonaje::ApuntarLiberado() {
	if(CombateComponente) {
		CombateComponente->ActualizarApuntando(false);
	}
}

void ADispareitorPersonaje::DispararPulsado() {
	if(CombateComponente) {
		CombateComponente->DispararPresionado(true);
	}
}

void ADispareitorPersonaje::DispararLiberado() {
	if(CombateComponente) {
		CombateComponente->DispararPresionado(false);
	}
}

void ADispareitorPersonaje::Recargar() {
	if(CombateComponente) {
		CombateComponente->Recargar();
	}
}

void ADispareitorPersonaje::Equipar() {
	if(CombateComponente) {
		if(HasAuthority()) { // Estamos en el servidor
			CombateComponente->EquiparArma(ArmaSolapada);
		} else { // Estamos en un cliente
			ServidorEquipar();
		}		
	}
}

// Aunque la definicion de la funcion es ServidorEquipar hay que añadirle _Implementation, ya que UE creará ServidorEquipar y nosotros _Implementation que incluirá el codigo que se ejecuta en el servidor  
void ADispareitorPersonaje::ServidorEquipar_Implementation() {
	if(CombateComponente) {
		CombateComponente->EquiparArma(ArmaSolapada);
	}
}

// En el servidor (Para el caso en el que el jugador sea ademas el servidor)
// Sabemos que esta funcion solo es llamada en el servidor 
// Llamada por Arma al solapar y desolapar el arma
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
void ADispareitorPersonaje::AlReplicarArmaSolapada(AArma* ArmaReplicadaAnterior) {
	if(ArmaSolapada) {
		ArmaSolapada->MostrarLeyendaSobreArma(true);
	}
	if(ArmaReplicadaAnterior) {
		ArmaReplicadaAnterior->MostrarLeyendaSobreArma(false);
	}
}

bool ADispareitorPersonaje::EstaArmaEquipada() {
	return CombateComponente && CombateComponente->ArmaEquipada;
}

bool ADispareitorPersonaje::EstaApuntando() {
	return CombateComponente && CombateComponente->bApuntando;
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
			InterpolacionAOGiro = AOGiro;
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
		InterpolacionAOGiro = FMath::FInterpTo(InterpolacionAOGiro, 0.f, DeltaTime, 4.f);
		AOGiro = InterpolacionAOGiro;
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
void ADispareitorPersonaje::ProxiesSimuladosGiro() {
	if(CombateComponente == nullptr || CombateComponente->ArmaEquipada == nullptr) {
		return;
	}

	bRotarHuesoRaiz = false;

	float Velocidad = CalcularVelocidad();
	if(Velocidad > 0.f) {
		GirarEnSitio = EGirarEnSitio::EGES_NoGirar;		
		return;
	}

	ProxyRotacionFrameAnterior = ProxyRotacionFrameActual;
	ProxyRotacionFrameActual = GetActorRotation();
	ProxyGiro = UKismetMathLibrary::NormalizedDeltaRotator(ProxyRotacionFrameActual, ProxyRotacionFrameAnterior).Yaw;

	if(FMath::Abs(ProxyGiro) > GiroUmbral) {
		if(ProxyGiro > GiroUmbral) {
			GirarEnSitio = 	EGirarEnSitio::EGES_Derecha;
		} else if(ProxyGiro < -GiroUmbral) {
			GirarEnSitio = EGirarEnSitio::EGES_Izquierda;
		} else {
			GirarEnSitio = EGirarEnSitio::EGES_NoGirar;	
		}
	} else {
		GirarEnSitio = EGirarEnSitio::EGES_NoGirar;		
	}
}

AArma* ADispareitorPersonaje::ObtenerArmaEquipada() {
	return CombateComponente == nullptr ? nullptr : CombateComponente->ArmaEquipada;
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
		switch(CombateComponente->ArmaEquipada->TipoArmaObtener()) {
			case ETipoArma::ETA_RifleAsalto:
				NombreSeccion = FName("RifleAsalto");	
				break;
		}
		InstanciaAnimacion->Montage_JumpToSection(NombreSeccion);
	}
}


// Llamado por RecibirDano y AlReplicarVida
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
void ADispareitorPersonaje::EjecutarMontajeEliminacion() {
	UAnimInstance* InstanciaAnimacion = GetMesh()->GetAnimInstance();
	if(InstanciaAnimacion && MontajeEliminacion) {
		InstanciaAnimacion->Montage_Play(MontajeEliminacion);
	}
}

FVector ADispareitorPersonaje::ObtenerObjetoAlcanzado() const {
	return (CombateComponente == nullptr || CombateComponente->ArmaEquipada == nullptr) ? FVector() : CombateComponente->ObjetoAlcanzado;	
}

// Llamado por Tick
void ADispareitorPersonaje::EsconderCamaraSiPersonajeCerca() {
	if(!IsLocallyControlled()) {
		return;
	}
	if((Camara->GetComponentLocation() - GetActorLocation()).Size() < CamaraLimiteCerca) {
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

float ADispareitorPersonaje::CalcularVelocidad() {
	FVector VelocidadTemporal = GetVelocity();
    VelocidadTemporal.Z = 0.f;
    return VelocidadTemporal.Size();
}

// LLamado al recibir daño por parte de ProyectilBala
// Solo se ejecuta en el server
void ADispareitorPersonaje::RecibirDano(AActor* ActorDanado, float Dano, const UDamageType* TipoDano, class AController* ControladorInstigador, AActor* ActorCausante) {
	Vida = FMath::Clamp(Vida - Dano, 0.f, VidaMaxima);
	ActualizarHUDVida();
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
void ADispareitorPersonaje::AlReplicarVida() {
	ActualizarHUDVida();
	EjecutarMontajeReaccionAImpacto();
}

void ADispareitorPersonaje::ActualizarHUDVida() {
	DispareitorControladorJugador = DispareitorControladorJugador != nullptr ? DispareitorControladorJugador : Cast<ADispareitorControladorJugador>(Controller);
	if(DispareitorControladorJugador) {
		DispareitorControladorJugador->ActualizarHUDVida(Vida, VidaMaxima);
	}
}

// Llamado por DispareitorModoJuego::JugadorEliminado 
// Ejecutado en el server ya que DispareitorModoJuego solo existe en el server
void ADispareitorPersonaje::Eliminado() {
	if(CombateComponente && CombateComponente->ArmaEquipada) {
		CombateComponente->ArmaEquipada->Soltar();
	}
	MulticastEliminado();
	GetWorldTimerManager().SetTimer(TemporizadorEliminado, this, &ADispareitorPersonaje::TemporizadorEliminadoFinalizado, EliminadoRetardo);
}

void ADispareitorPersonaje::MulticastEliminado_Implementation() {
	bEliminado = true;
	EjecutarMontajeEliminacion();

	if(DisolucionInstanciaMaterial) {
		DisolucionInstanciaMaterialDinamico = UMaterialInstanceDynamic::Create(DisolucionInstanciaMaterial, this);
		GetMesh()->SetMaterial(0, DisolucionInstanciaMaterialDinamico);
		DisolucionInstanciaMaterialDinamico->SetScalarParameterValue(TEXT("Disolucion"), 0.55f);
		DisolucionInstanciaMaterialDinamico->SetScalarParameterValue(TEXT("Brillo"), 200.f);
		DisolucionEmpezar();
	}

	GetCharacterMovement()->DisableMovement(); // Impide movimiento
	GetCharacterMovement()->StopMovementImmediately(); // Impide rotacion
	if(DispareitorControladorJugador) { 
		DisableInput(DispareitorControladorJugador); // Impide disparar
	}

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	if(RobotEliminacionSistemaParticulas) {
		FVector RobotEliminacionPuntoAparicion(GetActorLocation().X, GetActorLocation().Y, GetActorLocation().Z + 200.f);
		RobotEliminacionComponente = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), RobotEliminacionSistemaParticulas, RobotEliminacionPuntoAparicion, GetActorRotation());
	}
	if(RobotEliminacionSonido) {
		UGameplayStatics::SpawnSoundAtLocation(this, RobotEliminacionSonido, GetActorLocation());
	}
	if(HUDSobreLaCabeza) {
		HUDSobreLaCabeza->SetVisibility(false);
	}
	if(DispareitorControladorJugador) {
		DispareitorControladorJugador->ActualizarHUDMunicionArma(0);
	}
}

void ADispareitorPersonaje::DisolucionEmpezar() {
	DisolucionRutaDelegado.BindDynamic(this, &ADispareitorPersonaje::DisolucionActualizarMaterialCallback);
	if(DisolucionLineaTiempoComponente && DisolucionCurva) {
		DisolucionLineaTiempoComponente->AddInterpFloat(DisolucionCurva, DisolucionRutaDelegado);
		DisolucionLineaTiempoComponente->Play();
	}
}

void ADispareitorPersonaje::DisolucionActualizarMaterialCallback(float DisolucionValor) {
	if(DisolucionInstanciaMaterialDinamico) {
		DisolucionInstanciaMaterialDinamico->SetScalarParameterValue(TEXT("Disolucion"), DisolucionValor);
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

	if(RobotEliminacionComponente) {
		RobotEliminacionComponente->DestroyComponent();
	}
}

EEstadosCombate ADispareitorPersonaje::EstadoCombateObtener() const {
	return CombateComponente == nullptr ? EEstadosCombate::EEC_Maximo : CombateComponente->EstadoCombate;
}

