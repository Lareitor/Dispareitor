#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Dispareitor/Tipos/GirarEnSitio.h"
#include "Dispareitor/Interfaces/InteractuarConCrucetaInterfaz.h"
#include "Components/TimelineComponent.h"
#include "Dispareitor/Tipos/EstadosCombate.h"
#include "DispareitorPersonaje.generated.h"

// TODO Las granadas iniciales no se muestran en ADispareitorControladorJugador::SondearInicio
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
	virtual void OnRep_ReplicatedMovement() override;
	void Eliminado();
	// RPC Multicast. Si se invoca en el servidor, se ejecuta en el servidor + clientes, si se invoca en el cliente solo se ejecuta en ese cliente
	UFUNCTION(NetMulticast, Reliable) void Eliminado_Multicast();
	virtual void Destroyed() override;
	void SondearInicializacion();
	UPROPERTY(Replicated) bool bSoloGirarCamara = false;
	void ActualizarVidaHUD();
	void ActualizarEscudoHUD();
	void ActualizarMunicionHUD();
	void ReaparecerArmaPorDefecto();

protected:
	virtual void BeginPlay() override;
	void MoverAdelanteAtrasPulsado(float Valor);
	void MoverIzquierdaDerechaPulsado(float Valor);
	void GirarIzquierdaDerechaPulsado(float Valor);
	void GirarArribaAbajoPulsado(float Valor);
	virtual void Jump() override;
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

private:
	// Inicializa la variable a nullptr, en otro caso tendria basura y podría producir errores de codigo como crasheos. Otra forma de hacerlo seria usando directamente = nullptr;
	UPROPERTY() class ADispareitorControladorJugador* DispareitorControladorJugador; 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true")) class UCombateComponente* CombateComponente;
	UPROPERTY(VisibleAnywhere) class UBuffComponente* BuffComponente;
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
	UFUNCTION(Server, Reliable) void Equipar_EnServidor();
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
	UPROPERTY(VisibleAnywhere, Category = Eliminacion) UMaterialInstanceDynamic* InstanciaMaterialDinamicoParaDisolucion;
	// Se setea en el BP y a partir de ella creamos InstanciaMaterialDinamicoParaDisolucion
	UPROPERTY(EditAnywhere, Category = Eliminacion)	UMaterialInstance* InstanciaMaterialParaDisolucion;
	/**
	 * Robot eliminacion
	 */
	UPROPERTY(EditAnywhere)	UParticleSystem* SistemaParticulasRobotEliminacion; //p_bot_cage
	// Almacena el SistemaParticulasRobotEliminacion creado
	UPROPERTY(VisibleAnywhere) UParticleSystemComponent* ComponenteSistemaParticulasRobotEliminacion;
	UPROPERTY(EditAnywhere)	class USoundCue* SonidoRobotEliminacion; //roboteliminacion_cue
	UPROPERTY(VisibleAnywhere) UStaticMeshComponent* Granada;
	// Arma por defecto
	UPROPERTY(EditAnywhere) TSubclassOf<AArma> ClaseArmaPorDefecto;

public:	
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

};
