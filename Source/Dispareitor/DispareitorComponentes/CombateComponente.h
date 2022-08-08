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
	void IntercambiarArmas();
	void Recargar();
	UFUNCTION(BluePrintCallable) void RecargarFinalizado();
	UFUNCTION(BluePrintCallable) void RecargarCartuchoEscopeta();
	void DispararPresionado(bool bPresionado);
	void SaltarAFinAnimacionEscopeta();
	UFUNCTION(BluePrintCallable) void ArrojarGranadaFinalizado();
	UFUNCTION(BluePrintCallable) void GranadaArrojada();
	UFUNCTION(Server, Reliable)	void GranadaArrojada_EnServidor(const FVector_NetQuantize& Objetivo);
	void CogerMunicion(ETipoArma TipoArma, int32 IncrementoMunicion);

protected:	
	virtual void BeginPlay() override;
	void ActualizarApuntando(bool Apuntado);
	UFUNCTION(Server, Reliable)	void ActualizarApuntando_EnServidor(bool Apuntando);
	UFUNCTION() void AlReplicar_ArmaEquipada();	
	UFUNCTION() void AlReplicar_ArmaSecundariaEquipada();
	UFUNCTION(Server, Reliable)	void Disparar_EnServidor(const FVector_NetQuantize& Objetivo);
	UFUNCTION(Server, Reliable)	void Recargar_EnServidor();
	// RPC Multicast. Si se invoca en el servidor, se ejecuta en el servidor + clientes, si se invoca en el cliente solo se ejecuta en ese cliente
	UFUNCTION(NetMulticast, Reliable) void Disparar_Multicast(const FVector_NetQuantize& Objetivo);
	void CalcularRayoDesdeCruceta(FHitResult& RayoResultado);
	void ActualizarCrucetaHUD(float DeltaTime);
	void EjecutarMontajeRecargar();
	int32 CalcularCantidadARecargar();
	UPROPERTY(EditAnywhere)	class USoundCue* SonidoFrancotiradorCrucetaZoomIn; //SniperRifle
	UPROPERTY(EditAnywhere)	USoundCue* SonidoFrancotiradorCrucetaZoomOut;
	void ArrojarGranada();
	UFUNCTION(Server, Reliable)	void ArrojarGranada_EnServidor();
	UPROPERTY(EditAnywhere)	TSubclassOf<class AProyectil> GranadaClase;
	void SoltarArmaEquipada();
	void UnirActorAManoDerecha(AActor* Actor);
	void UnirActorAManoIzquierda(AActor* Actor);
	void UnirActorAMochila(AActor* Actor);
	void ActualizarMunicionPersonaje();
	void RecargarArmaVacia();
	void MostrarGranada(bool bMostrar);
	void EquiparArmaPrimaria(AArma* ArmaAEquipar);
	void EquiparArmaSecundaria(AArma* ArmaAEquipar);

private:
	UPROPERTY()	class ADispareitorPersonaje* DispareitorPersonaje;
	UPROPERTY()	class ADispareitorControladorJugador* DispareitorControladorJugador;
	UPROPERTY()	class ADispareitorHUD* DispareitorHUD;
	UPROPERTY(ReplicatedUsing = AlReplicar_ArmaEquipada) AArma* ArmaEquipada;
	UPROPERTY(ReplicatedUsing = AlReplicar_ArmaSecundariaEquipada) AArma* ArmaSecundariaEquipada;
	UPROPERTY(Replicated) bool bApuntando;
	UPROPERTY(EditAnywhere)	float VelocidadCaminarBase;
	UPROPERTY(EditAnywhere)	float VelocidadCaminarApuntando;
	bool bDispararPresionado;
	float CrucetaFactorVelocidad;
	float CrucetaFactorEnAire;
	float CrucetaFactorApuntado;
	float CrucetaFactorDisparo;
	FVector ObjetoAlcanzado;
	FHUDCruceta HUDCruceta;
	float FOVPorDefecto;
	UPROPERTY(EditAnywhere, Category = Combate)	float ZoomFOV =  30.f;
	float ActualFOV;
	UPROPERTY(EditAnywhere, Category = Combate)	float ZoomVelocidadInterpolacion = 20.f;
	void InterpolarFOV(float DeltaTime);
	FTimerHandle TemporizadorDisparo;
	bool bPuedoDisparar = true;
	void Disparar();
	void EmpezarTemporizadorDisparo();
	void TerminadoDisparoTemporizador();
	bool PuedoDisparar();
	// Municion del personaje para el arma actualmente equipada
	UPROPERTY(ReplicatedUsing = AlReplicar_MunicionPersonaje) int32 MunicionPersonaje;
	UFUNCTION()
	void AlReplicar_MunicionPersonaje();
	// TMap no puede replicarse
	TMap<ETipoArma, int32> MapaMunicionPersonaje;
	UPROPERTY(EditAnywhere)	int32 MaximaMunicionPersonaje = 500;
	UPROPERTY(EditAnywhere)	int32 MunicionPersonajeInicialRifleAsalto = 30;
	UPROPERTY(EditAnywhere)	int32 MunicionPersonajeInicialLanzaCohetes = 0;
	UPROPERTY(EditAnywhere)	int32 MunicionPersonajeInicialPistola = 0;
	UPROPERTY(EditAnywhere)	int32 MunicionPersonajeInicialSubfusil = 0;
	UPROPERTY(EditAnywhere)	int32 MunicionPersonajeInicialEscopeta = 0;
	UPROPERTY(EditAnywhere)	int32 MunicionPersonajeInicialFrancotirador = 0;
	UPROPERTY(EditAnywhere)	int32 MunicionPersonajeInicialLanzaGranadas = 0;
	void InicializarMunicionPersonaje();
	UPROPERTY(ReplicatedUsing = AlReplicar_EstadoCombate) EEstadosCombate EstadoCombate = EEstadosCombate::EEC_Desocupado;
	UFUNCTION()	void AlReplicar_EstadoCombate();
	void ActualizarValoresMunicion();
	void ActualizarValoresMunicionEscopeta();
	void EjecutarSonidoAlEquipar(AArma* ArmaAEquipar);
	UPROPERTY(ReplicatedUsing = AlReplicar_GranadasActuales) int32 GranadasActuales = 4;
	UFUNCTION()	void AlReplicar_GranadasActuales();
	UPROPERTY(EditAnywhere)	int32 GranadasMaximo = 4;
	void ActualizarGranadasHUD();
	
public:		
	FORCEINLINE int32 ObtenerGranadasActuales() const { return GranadasActuales; }
	FORCEINLINE int32 ObtenerMunicionPersonaje() const { return MunicionPersonaje; }
	FORCEINLINE AArma* ObtenerArmaEquipada() const { return ArmaEquipada; }
	bool PuedoIntercambiarArmas();
};
