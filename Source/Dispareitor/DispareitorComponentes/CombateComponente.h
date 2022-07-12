#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Dispareitor/HUD/DispareitorHUD.h"
#include "Dispareitor/Tipos/TiposArma.h"
#include "Dispareitor/Tipos/EstadosCombate.h"
#include "CombateComponente.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DISPAREITOR_API UCombateComponente : public UActorComponent {
	GENERATED_BODY()

public:		
	UCombateComponente();
	// Ya que estan relacionadas, definimos ADispareitorPersonaje como clase amiga para que pueda acceder a sus metodos y variables protegidas y privadas
	friend class ADispareitorPersonaje;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;			
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void EquiparArma(class AArma* ArmaAEquipar);
	void Recargar();

	UFUNCTION(BluePrintCallable)
	void RecargarFinalizado();
	
	void DispararPresionado(bool bPresionado);

protected:	
	virtual void BeginPlay() override;
	void ActualizarApuntando(bool Apuntado);

	UFUNCTION(Server, Reliable)
	void ServidorActualizarApuntando(bool Apuntando);

	UFUNCTION()
	void AlReplicarArmaEquipada();	

	UFUNCTION(Server, Reliable)
	void ServidorDisparar(const FVector_NetQuantize& Objetivo);

	UFUNCTION(Server, Reliable)
	void RecargarServidor();

	// RPC Multicast. Si se invoca en el servidor, se ejecuta en el servidor + clientes, si se invoca en el cliente solo se ejecuta en ese cliente
	UFUNCTION(NetMulticast, Reliable)
	void MulticastDisparar(const FVector_NetQuantize& Objetivo);

	void CalcularRayoDesdeCruceta(FHitResult& RayoResultado);

	void ActualizarHUDCruceta(float DeltaTime);

	void RecargarManejador();

	int32 RecargarCantidad();

private:
	UPROPERTY()
	class ADispareitorPersonaje* DispareitorPersonaje;
	
	UPROPERTY()
	class ADispareitorControladorJugador* DispareitorControladorJugador;
	
	UPROPERTY()
	class ADispareitorHUD* DispareitorHUD;

	UPROPERTY(ReplicatedUsing = AlReplicarArmaEquipada)
	AArma* ArmaEquipada;

	UPROPERTY(Replicated)
	bool bApuntando;

	UPROPERTY(EditAnywhere)
	float VelocidadCaminarBase;

	UPROPERTY(EditAnywhere)
	float VelocidadCaminarApuntando;

	bool bDispararPresionado;

	float CrucetaFactorVelocidad;
	float CrucetaFactorEnAire;
	float CrucetaFactorApuntado;
	float CrucetaFactorDisparo;

	FVector ObjetoAlcanzado;

	FHUDCruceta HUDCruceta;

	float PorDefectoFOV;

	UPROPERTY(EditAnywhere, Category = Combate)
	float ZoomFOV =  30.f;

	float ActualFOV;

	UPROPERTY(EditAnywhere, Category = Combate)
	float ZoomVelocidadInterpolacion = 20.f;

	void InterpolarFOV(float DeltaTime);

	FTimerHandle DisparoTemporizador;
	bool bPuedoDisparar = true;

	void Disparar();
	void EmpezarDisparoTemporizador();
	void TerminadoDisparoTemporizador();

	bool PuedoDisparar();

	// Municion del personaje para el arma actualmente equipada
	UPROPERTY(ReplicatedUsing = MunicionPersonajeAlReplicar)
	int32 MunicionPersonaje;

	UFUNCTION()
	void MunicionPersonajeAlReplicar();

	// TMap no puede replicarse
	TMap<ETipoArma, int32> MunicionPersonajeMapa;

	UPROPERTY(EditAnywhere)
	int32 MunicionPersonajeInicialRifleAsalto = 30;

	UPROPERTY(EditAnywhere)
	int32 MunicionPersonajeInicialLanzaCohetes = 0;

	UPROPERTY(EditAnywhere)
	int32 MunicionPersonajeInicialPistola = 0;

	UPROPERTY(EditAnywhere)
	int32 MunicionPersonajeInicialSubfusil = 0;
	
	UPROPERTY(EditAnywhere)
	int32 MunicionPersonajeInicialEscopeta = 0;

	UPROPERTY(EditAnywhere)
	int32 MunicionPersonajeInicialFrancotirador = 0;

	void MunicionPersonajeInicializar();

	UPROPERTY(ReplicatedUsing = EstadoCombateAlReplicar)
	EEstadosCombate EstadoCombate = EEstadosCombate::EEC_Desocupado;

	UFUNCTION()
	void EstadoCombateAlReplicar();

	void MunicionActualizarValores();

	void EquiparSonido();

	bool ArmaSinMunicionPeroPuedoRecargar();
	
public:		
	
};
