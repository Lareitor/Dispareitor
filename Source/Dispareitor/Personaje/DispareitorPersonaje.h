#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Dispareitor/Tipos/GirarEnSitio.h"
#include "Dispareitor/Interfaces/InteractuarConCrucetaInterfaz.h"
#include "Components/TimelineComponent.h"
#include "Dispareitor/Tipos/EstadosCombate.h"
#include "DispareitorPersonaje.generated.h"

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
	void EjecutarMontajeEliminacion();
	virtual void OnRep_ReplicatedMovement() override;
	void Eliminado();

	// RPC Multicast. Si se invoca en el servidor, se ejecuta en el servidor + clientes, si se invoca en el cliente solo se ejecuta en ese cliente
	UFUNCTION(NetMulticast, Reliable)
	void MulticastEliminado();
	virtual void Destroyed() override;

	void SondearInicializacion();

protected:
	virtual void BeginPlay() override;
	void MoverAdelanteAtras(float Valor);
	void MoverIzquierdaDerecha(float Valor);
	void Girar(float Valor);
	void MirarArribaAbajo(float Valor);
	virtual void Jump() override;
	void Agachar();
	void Equipar();
	void ApuntarPulsado();
	void ApuntarLiberado();
	void DispararPulsado();
	void DispararLiberado();
	void Recargar();
	void CalcularGiroEInclinacionParadoYArmado(float DeltaTime);
	void CalcularInclinacion();
	void ProxiesSimuladosGiro();
	void EjecutarMontajeReaccionAImpacto();
	
	UFUNCTION()
	void RecibirDano(AActor* ActorDanado, float Dano, const UDamageType* TipoDano, class AController* ControladorInstigador, AActor* ActorCausante);
	void ActualizarHUDVida();

private:
	UPROPERTY(VisibleAnywhere, Category = Camara)	
	class USpringArmComponent* BrazoCamara;

	UPROPERTY(VisibleAnywhere, Category = Camara)	
	class UCameraComponent* Camara;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UWidgetComponent* HUDSobreLaCabeza;

	// Cuando cambie esta variable en el servidor se replicará su estado automaticamente en los clientes seleccionados
	// La replicación funciona sola en una direccion servidor -> clientes
	// La replicacion es mas eficiente que las llamadas RPCs
	UPROPERTY(ReplicatedUsing = AlReplicarArmaSolapada) 
	class AArma* ArmaSolapada;
		
	UFUNCTION()
	void AlReplicarArmaSolapada(AArma* ArmaReplicadaAnterior);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UCombateComponente* CombateComponente;

	// Hace esta funcion de tipo RPC, para llamarla desde los clientes pero que se ejecuten en el servidor
	// Reliable garantiza que la información llega al servidor (el cliente recibe una confirmación de parte del servidor, si no la recibe vuelve a enviar la info)
	UFUNCTION(Server, Reliable) 
	void ServidorEquipar();

	// AO = Aim Offset
	float AOGiro;
	float InterpolacionAOGiro;
	float AOInclinacion;
	FRotator ArmadoRotacionInicial;

	// Indica el sentido en el que debemos realizar el giro cuando estamos parados y con arma. Se utiliza en las transacciones de la maquina de estados
	EGirarEnSitio GirarEnSitio;
	void CalcularGirarEnSitio(float DeltaTime);

	UPROPERTY(EditAnywhere, Category = Combate)
	class UAnimMontage* MontajeDispararArma;

	UPROPERTY(EditAnywhere, Category = Combate)
	class UAnimMontage* MontajeRecargar;

	UPROPERTY(EditAnywhere, Category = Combate)
	class UAnimMontage* MontajeReaccionAImpacto;

	UPROPERTY(EditAnywhere, Category = Combate)
	class UAnimMontage* MontajeEliminacion;

	void EsconderCamaraSiPersonajeCerca();

	UPROPERTY(EditAnywhere)
	float CamaraLimiteCerca = 200.f;

	// Al rotar el hueso raiz giramos la cintura sin mover las piernas
	bool bRotarHuesoRaiz;
	// Para los proxies simulados
	float GiroUmbral = 0.5f;
	FRotator ProxyRotacionFrameAnterior;
	FRotator ProxyRotacionFrameActual;
	float ProxyGiro;
	float TiempoDesdeUltimaReplicacionDeMovimiento;
	
	float CalcularVelocidad();

	/**
	 * Vida 
	 */
	UPROPERTY(EditAnywhere, Category = "Estadisticas")
	float VidaMaxima = 100.f;

	UPROPERTY(ReplicatedUsing = AlReplicarVida, VisibleAnywhere, Category = "Estadisticas")
	float Vida = 100.f;

	UFUNCTION()
	void AlReplicarVida();

	UPROPERTY() // Inicializa la variable a nullptr, en otro caso tendria basura y podría producir errores de codigo como crasheos. Otra forma de hacerlo seria usando directamente = nullptr;
	class ADispareitorControladorJugador* DispareitorControladorJugador; 

	bool bEliminado = false;

	FTimerHandle TemporizadorEliminado;
	
	UPROPERTY(EditDefaultsOnly) // Solo editable en la clase Character?¿ para que no se pueda editar en las hijas y que puedan poner distintos retardos lo cual no sería justo
	float EliminadoRetardo = 3.f;

	void TemporizadorEliminadoFinalizado();

	/** 
	 * Efecto disolucion
	 */
	UPROPERTY(VisibleAnywhere)
	UTimelineComponent* DisolucionLineaTiempoComponente;
	
	FOnTimelineFloat DisolucionRutaDelegado;

	UPROPERTY(EditAnywhere)
	UCurveFloat* DisolucionCurva;

	UFUNCTION()
	void DisolucionActualizarMaterialCallback(float DisolucionValor);

	void DisolucionEmpezar();

	UPROPERTY(VisibleAnywhere, Category = Eliminacion)
	UMaterialInstanceDynamic* DisolucionInstanciaMaterialDinamico;

	// Se setea en el BP y a partir de ella creamos DisolucionInstanciaMaterialDinamico
	UPROPERTY(EditAnywhere, Category = Eliminacion)
	UMaterialInstance* DisolucionInstanciaMaterial;

	/**
	 * Robot eliminacion
	 */
	UPROPERTY(EditAnywhere)
	UParticleSystem* RobotEliminacionSistemaParticulas;

	// Almacena el RobotEliminacionSistemaParticulas creado
	UPROPERTY(VisibleAnywhere)
	UParticleSystemComponent* RobotEliminacionComponente;

	UPROPERTY(EditAnywhere)
	class USoundCue* RobotEliminacionSonido;

	UPROPERTY()
	class ADispareitorEstadoJugador* DispareitorEstadoJugador;

public:	
	void ActivarArmaSolapada(AArma* Arma);
	bool EstaArmaEquipada();
	bool EstaApuntando();
	FORCEINLINE float ObtenerAOGiro() const { return AOGiro; } 
	FORCEINLINE float ObtenerAOInclinacion() const { return AOInclinacion; } 
	AArma* ObtenerArmaEquipada();
	FORCEINLINE EGirarEnSitio ObtenerGirarEnSitio() const { return GirarEnSitio; }
	FVector ObtenerObjetoAlcanzado() const;
	FORCEINLINE UCameraComponent* ObtenerCamara() const { return Camara; }
	FORCEINLINE bool DeboRotarHuesoRaiz() const { return bRotarHuesoRaiz; }
	FORCEINLINE bool EstaEliminado() const { return bEliminado; }
	FORCEINLINE float ObtenerVida() const { return Vida; }
	FORCEINLINE float ObtenerVidaMaxima() const { return VidaMaxima; }
	EEstadosCombate EstadoCombateObtener() const;
};
