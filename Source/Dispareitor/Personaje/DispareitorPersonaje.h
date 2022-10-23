#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Dispareitor/Tipos/GirarEnSitio.h"
#include "Dispareitor/Interfaces/InteractuarConCrucetaInterfaz.h"
#include "Components/TimelineComponent.h"
#include "Dispareitor/Tipos/EstadosCombate.h"
#include "Dispareitor/Tipos/Equipo.h"
#include "DispareitorPersonaje.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDelegadoDejarJuego);

// TODO 
// Cuando las armas se caen al vacio volver a reponerlas 
// Si la cruceta no se pone roja al pasar sobre un enemigo, activar manualmente el check Trace Responses a Block en la malla del personaje
// Si los disparos no son precisos a la malla comprobar que en la malla el tipo de canal es MallaDelEsqueleto
UCLASS()
class DISPAREITOR_API ADispareitorPersonaje : public ACharacter, public IInteractuarConCrucetaInterfaz {
	GENERATED_BODY()

public:	
	ADispareitorPersonaje();	
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PostInitializeComponents() override;
	void EjecutarMontajeDispararArma(bool bApuntando); 
	void EjecutarMontajeRecargar(); 
	void EjecutarMontajeEliminado(); 
	void EjecutarMontajeArrojarGranada(); 
	void EjecutarMontajeIntercambiarArmas();
	virtual void OnRep_ReplicatedMovement() override;
	void Eliminado(bool bJugadorDejoJuego);
	// RPC Multicast. Si se invoca en el servidor, se ejecuta en el servidor + clientes, si se invoca en el cliente solo se ejecuta en ese cliente
	UFUNCTION(NetMulticast, Reliable) void Eliminado_Multicast(bool bJugadorDejoJuego);
	virtual void Destroyed() override;
	void SondearInicializacion();
	UPROPERTY(Replicated) bool bSoloGirarCamara = false;
	void ActualizarVidaHUD();
	void ActualizarEscudoHUD();
	void ActualizarMunicionHUD();
	void ActualizarGranadasHUD();
	void ReaparecerArmaPorDefecto();
	UPROPERTY() TMap<FName, class UBoxComponent*> CajasColision; 
	bool bIntercambiarArmasFinalizado = true;
	UFUNCTION(Server, Reliable) void DejarJuego_EnServidor();
	FDelegadoDejarJuego DelegadoDejarJuego;
	UFUNCTION(NetMulticast, Reliable) void GanoElLider_Multicast();
	UFUNCTION(NetMulticast, Reliable) void PerdioElLider_Multicast();
	void ActivarColorEquipo(EEquipo Equipo);

protected:
	virtual void BeginPlay() override;
	void MoverAdelanteAtrasPulsado(float Valor);
	void MoverIzquierdaDerechaPulsado(float Valor);
	void GirarIzquierdaDerechaPulsado(float Valor);
	void GirarArribaAbajoPulsado(float Valor);
	void AgacharPulsado();
	void EquiparPulsado();
	void ApuntarPulsado();
	void ApuntarLiberado();
	void DispararPulsado();
	void DispararLiberado();
	void RecargarPulsado();
	void ArrojarGranadaPulsado();
	void CalcularRotarEnSitio(float DeltaTime);
	void CalcularGiroEInclinacionParadoYArmado(float DeltaTime);
	void CalcularInclinacion();
	void CalcularGiroParadoYArmadoEnProxiesSimulados();
	void EjecutarMontajeReaccionAImpacto();
	UFUNCTION() void RecibirDanio(AActor* ActorDaniado, float Danio, const UDamageType* TipoDanio, class AController* ControladorInstigador, AActor* ActorCausante);
	void SoltarODestruirArma(AArma* Arma);
	void SoltarODestruirArmas();
	// Cajas de impacto usadas para server-side rewind
	UPROPERTY(EditAnywhere)	class UBoxComponent* CajaCabeza;
	UPROPERTY(EditAnywhere)	class UBoxComponent* CajaPelvis; 
	UPROPERTY(EditAnywhere)	class UBoxComponent* CajaEspina02;
	UPROPERTY(EditAnywhere)	class UBoxComponent* CajaEspina03;
	UPROPERTY(EditAnywhere)	class UBoxComponent* CajaBrazoI;
	UPROPERTY(EditAnywhere)	class UBoxComponent* CajaBrazoD;
	UPROPERTY(EditAnywhere)	class UBoxComponent* CajaAntebrazoI;
	UPROPERTY(EditAnywhere)	class UBoxComponent* CajaAntebrazoD;
	UPROPERTY(EditAnywhere)	class UBoxComponent* CajaManoI;
	UPROPERTY(EditAnywhere)	class UBoxComponent* CajaManoD;
	UPROPERTY(EditAnywhere)	class UBoxComponent* CajaMochila;
	UPROPERTY(EditAnywhere)	class UBoxComponent* CajaManta;
	UPROPERTY(EditAnywhere)	class UBoxComponent* CajaMusloI;
	UPROPERTY(EditAnywhere)	class UBoxComponent* CajaMusloD;
	UPROPERTY(EditAnywhere)	class UBoxComponent* CajaGemeloI;
	UPROPERTY(EditAnywhere)	class UBoxComponent* CajaGemeloD;
	UPROPERTY(EditAnywhere)	class UBoxComponent* CajaPieI;
	UPROPERTY(EditAnywhere)	class UBoxComponent* CajaPieD;

private:
	// Inicializa la variable a nullptr, en otro caso tendria basura y podría producir errores de codigo como crasheos. Otra forma de hacerlo seria usando directamente = nullptr;
	UPROPERTY() class ADispareitorControladorJugador* DispareitorControladorJugador; 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true")) class UCombateComponente* CombateComponente;
	UPROPERTY(VisibleAnywhere) class UBuffComponente* BuffComponente;
	UPROPERTY(VisibleAnywhere) class UCompensacionLagComponente* CompensacionLagComponente; 
	UPROPERTY()	class ADispareitorEstadoJugador* DispareitorEstadoJugador;
	UPROPERTY(VisibleAnywhere, Category = Camara) class USpringArmComponent* BrazoCamara;
	UPROPERTY(VisibleAnywhere, Category = Camara) class UCameraComponent* Camara;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true")) class UWidgetComponent* HUDSobreLaCabeza;
	UPROPERTY()	class UHUDSobreLaCabeza* HUDSobreLaCabezaReal;
	// Cuando cambie esta variable en el servidor se replicará su estado automaticamente en los clientes seleccionados
	// La replicación funciona sola en una direccion servidor -> clientes. La replicacion es mas eficiente que las llamadas RPCs
	UPROPERTY(ReplicatedUsing = ArmaSolapada_AlReplicar) class AArma* ArmaSolapada;
	UFUNCTION()	void ArmaSolapada_AlReplicar(AArma* ArmaReplicadaAnterior);
	// Hace esta funcion de tipo RPC, para llamarla desde los clientes pero que se ejecuten en el servidor
	// Reliable garantiza que la información llega al servidor (el cliente recibe una confirmación de parte del servidor, si no la recibe vuelve a enviar la info)
	UFUNCTION(Server, Reliable) void EquiparIntercambiar_EnServidor();
	/**
	 * AO = Aim Offset
	 */
	float AOGiro;
	float AOGiroInterpolacion;
	float AOInclinacion;
	FRotator ArmadoRotacionInicial;
	// Indica el sentido en el que debemos realizar el giro cuando estamos parados y con arma. Se utiliza en las transacciones de la maquina de estados
	EGirarEnSitio GirarEnSitio;
	void CalcularGirarEnSitio(float DeltaTime);
	UPROPERTY(EditAnywhere, Category = Combate)	class UAnimMontage* MontajeDispararArma;
	UPROPERTY(EditAnywhere, Category = Combate)	class UAnimMontage* MontajeRecargar;
	UPROPERTY(EditAnywhere, Category = Combate)	class UAnimMontage* MontajeReaccionAImpacto;
	UPROPERTY(EditAnywhere, Category = Combate)	class UAnimMontage* MontajeEliminado;
	UPROPERTY(EditAnywhere, Category = Combate)	class UAnimMontage* MontajeArrojarGranada;
	UPROPERTY(EditAnywhere, Category = Combate)	class UAnimMontage* MontajeIntercambiarArmas;
	void EsconderCamaraSiPersonajeCerca();
	UPROPERTY(EditAnywhere)	float DistanciaMinimaEntrePersonajeYCamara = 200.f;
	// Al rotar el hueso raiz giramos la cintura sin mover las piernas
	bool bRotarHuesoRaiz;
	// Para los proxies simulados
	float UmbralDeGiroEnProxy = 0.5f;
	FRotator RotacionFrameAnteriorEnProxy;
	FRotator RotacionFrameActualEnProxy;
	float GiroEnProxy;
	float TiempoDesdeUltimaReplicacionDeMovimiento;
	float CalcularVelocidad();
	/**
	 * Vida 
	 */
	UPROPERTY(EditAnywhere, Category = "Estadisticas") float VidaMaxima = 100.f;
	UPROPERTY(ReplicatedUsing = AlReplicar_Vida, VisibleAnywhere, Category = "Estadisticas") float Vida = 100.f;
	UFUNCTION()	void AlReplicar_Vida(float VidaAnterior);
	/**
	 * Escudo 
	 */
	UPROPERTY(EditAnywhere, Category = "Estadisticas") float EscudoMaximo = 100.f;
	UPROPERTY(ReplicatedUsing = AlReplicar_Escudo, EditAnywhere, Category = "Estadisticas") float Escudo = 0.f;
	UFUNCTION()	void AlReplicar_Escudo(float EscudoAnterior);
	bool bEliminado = false;
	FTimerHandle TemporizadorEliminado;
	// Solo editable en la clase Character?¿ para que no se pueda editar en las hijas y que puedan poner distintos retardos lo cual no sería justo
	UPROPERTY(EditDefaultsOnly) float RetardoDeEliminacion = 3.f;
	void TemporizadorEliminadoFinalizado();
	/** 
	 * Efecto disolucion
	 */
	UPROPERTY(VisibleAnywhere) UTimelineComponent* ComponenteLineaDelTiempoParaDisolucion;
	FOnTimelineFloat DelegadoLineaDelTiempoParaDisolucion;
	UPROPERTY(EditAnywhere)	UCurveFloat* CurvaDeDisolucion;
	UFUNCTION()	void Callback_ActualizarMaterialEnDisolucion(float DisolucionValor);
	void EmpezarDisolucion();
	UPROPERTY(VisibleAnywhere, Category = Eliminacion) UMaterialInstanceDynamic* IMDDisolucion;
	// A partir de ella creamos IMDDisolucion
	UPROPERTY(VisibleAnywhere, Category = Eliminacion)	UMaterialInstance* IMDisolucion;
	UPROPERTY(EditAnywhere, Category = Eliminacion)	UMaterialInstance* IMDisolucionEquipoRojo;
	UPROPERTY(EditAnywhere, Category = Eliminacion)	UMaterialInstance* IMDisolucionEquipoAzul;
	UPROPERTY(EditAnywhere, Category = Eliminacion)	UMaterialInstance* IMEquipoRojo;
	UPROPERTY(EditAnywhere, Category = Eliminacion)	UMaterialInstance* IMEquipoAzul;
	UPROPERTY(EditAnywhere, Category = Eliminacion)	UMaterialInstance* IMOriginal;

	/**
	 * Eliminacion
	 */
	UPROPERTY(EditAnywhere)	UParticleSystem* SistemaParticulasRobotEliminacion; //p_bot_cage
	// Almacena el SistemaParticulasRobotEliminacion creado
	UPROPERTY(VisibleAnywhere) UParticleSystemComponent* ComponenteSistemaParticulasRobotEliminacion;
	UPROPERTY(EditAnywhere)	class USoundCue* SonidoRobotEliminacion; //roboteliminacion_cue
	UPROPERTY(VisibleAnywhere) UStaticMeshComponent* Granada;
	UPROPERTY(EditAnywhere) class UNiagaraSystem* SistemaNiagaraCorona;
	UPROPERTY() class UNiagaraComponent* ComponenteNiagaraCorona;
	// Arma por defecto
	UPROPERTY(EditAnywhere) TSubclassOf<AArma> ClaseArmaPorDefecto;

	bool bDejarJuego = false;

public:	
	virtual void Jump() override;
	void ActivarArmaSolapada(AArma* Arma);
	bool HayArmaEquipada();
	bool EstaApuntando();
	AArma* ObtenerArmaEquipada();
	FVector ObtenerObjetoAlcanzado() const;
	EEstadosCombate ObtenerEstadoCombate() const;
	FORCEINLINE float ObtenerAOGiro() const { return AOGiro; } 
	FORCEINLINE float ObtenerAOInclinacion() const { return AOInclinacion; } 
	FORCEINLINE EGirarEnSitio ObtenerGirarEnSitio() const { return GirarEnSitio; }
	FORCEINLINE UCameraComponent* ObtenerCamara() const { return Camara; }
	FORCEINLINE bool DeboRotarHuesoRaiz() const { return bRotarHuesoRaiz; }
	FORCEINLINE bool EstaEliminado() const { return bEliminado; }
	FORCEINLINE float ObtenerVida() const { return Vida; }
	FORCEINLINE void ActualizarVida(float _Vida) { Vida = _Vida; }
	FORCEINLINE float ObtenerVidaMaxima() const { return VidaMaxima; }
	FORCEINLINE float ObtenerEscudo() const { return Escudo; }
	FORCEINLINE void ActualizarEscudo(float _Escudo) { Escudo = _Escudo; }
	FORCEINLINE float ObtenerEscudoMaximo() const { return EscudoMaximo; }
	FORCEINLINE bool DeboSoloGirarCamara() const { return bSoloGirarCamara; }
	FORCEINLINE UAnimMontage* ObtenerMontajeRecargar() const { return MontajeRecargar; } 
	FORCEINLINE UStaticMeshComponent* ObtenerGranada() const { return Granada; } 
	FORCEINLINE UCombateComponente* ObtenerCombateComponente() const { return CombateComponente; }
	FORCEINLINE UBuffComponente* ObtenerBuffComponente() const { return BuffComponente; } 
	bool EstaRecargandoLocalmente();
	FORCEINLINE UCompensacionLagComponente* ObtenerCompensacionLagComponente() const { return CompensacionLagComponente; } 
};
