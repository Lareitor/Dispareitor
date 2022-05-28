#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Dispareitor/Tipos/GirarEnSitio.h"
#include "Dispareitor/Interfaces/InteractuarConCrucetaInterfaz.h"
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

	// RPC Multicast. Si se invoca en el servidor, se ejecuta en el servidor + clientes, si se invoca en el cliente solo se ejecuta en ese cliente
	UFUNCTION(NetMulticast, Unreliable) // Como no hacemos nada importante, es solo cosmetico le indicamos que sea unreliable
	void MulticastImpacto();

	virtual void OnRep_ReplicatedMovement() override;

protected:
	virtual void BeginPlay() override;
	void MoverAdelanteAtras(float Valor);
	void MoverIzquierdaDerecha(float Valor);
	void Girar(float Valor);
	void MirarArribaAbajo(float Valor);
	void Equipar();
	void Agachar();
	void ApuntarPulsado();
	void ApuntarLiberado();
	void CalcularGiroEInclinacionParadoYArmado(float DeltaTime);
	void CalcularInclinacion();
	void ProxiesSimuladosGiro();
	virtual void Jump() override;
	void DispararPulsado();
	void DispararLiberado();
	void EjecutarMontajeReaccionAImpacto();

private:
	UPROPERTY(VisibleAnywhere, Category = Camara)	
	class USpringArmComponent* BrazoCamara;

	UPROPERTY(VisibleAnywhere, Category = Camara)	
	class UCameraComponent* Camara;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UWidgetComponent* HUDSobreLaCabeza;

	// Cuando cambie esta variable en el servidor se replicará su estado automaticamente en los clientes seleccionados
	// La replicación funciona sola en una direccion servidor -> clientes
	UPROPERTY(ReplicatedUsing = AlReplicarArmaSolapada) 
	class AArma* ArmaSolapada;
		
	UFUNCTION()
	void AlReplicarArmaSolapada(AArma* ArmaReplicadaAnterior);

	UPROPERTY(VisibleAnywhere)
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

	EGirarEnSitio GirarEnSitio;
	void CalcularGirarEnSitio(float DeltaTime);

	UPROPERTY(EditAnywhere, Category = Combate)
	class UAnimMontage* MontajeDispararArma;

	UPROPERTY(EditAnywhere, Category = Combate)
	class UAnimMontage* MontajeReaccionAImpacto;

	void EsconderCamaraSiPersonajeCerca();

	UPROPERTY(EditAnywhere)
	float CamaraLimiteCerca = 200.f;

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

	class ADispareitorControladorJugador* DispareitorControladorJugador;

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
};
