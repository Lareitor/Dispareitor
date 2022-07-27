#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "DispareitorControladorJugador.generated.h"

UCLASS()
class DISPAREITOR_API ADispareitorControladorJugador : public APlayerController {
	GENERATED_BODY()

public:
	void ActualizarVidaHUD(float Vida, float VidaMaxima);
	void ActualizarMuertosHUD(float Muertos);
	void ActualizarMuertesHUD(int32 Muertes);
	void ActualizarMunicionArmaHUD(int32 MunicionArma);
	void ActualizarMunicionPersonajeHUD(int32 MunicionPersonaje);
	void ActualizarTiempoPartidaHUD(float TiempoCuentaAtras);
	void ActualizarTiempoAnunciosHUD(float TiempoCuentaAtras);
	void ActualizarCrucetaFrancotiradorHUD(bool bEstaApuntando);
	void ActualizarGranadasHUD(int32 CantidadGranadas);
	virtual void OnPossess(APawn* Peon) override;
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual float ObtenerTiempoServidor();
	virtual void ReceivedPlayer() override;
	void ActualizarEstadoPartida(FName Estado);
	void ManejarEstadoPartida();
	void ManejarEnfriamiento();
	
protected:
	virtual void BeginPlay() override;
	void ActivarTiempoHUD();
	void SondearInicio();
	// Sincronizacion tiempos entre cliente y servidor
	UFUNCTION(Server, Reliable)	void PedirTiempoServidor_EnServidor(float TiempoClientePeticion);
	UFUNCTION(Client, Reliable)	void DevolverTiempoServidor_EnCliente(float TiempoClientePeticion, float TiempoServidorAlRecibirPeticion);
	float DeltaTiempoServidorCliente = 0.f; 
	UPROPERTY(EditAnywhere, Category = Tiempo) float TiempoSincronizacionFrecuencia = 5.f;
	float TiempoSincronizacionPasado = 0.f;
	void ComprobarTiempoSincronizacion(float DeltaTime);
	UFUNCTION(Server, Reliable)	void ComprobarEstadoPartida_EnServidor();
	UFUNCTION(Client, Reliable)	void ComprobarEstadoPartida_EnCliente(FName _EstadoPartida, float _TiempoCalentamiento, float _TiempoPartida, float _TiempoEnfriamiento, float _TiempoInicioNivel);

private:
	UPROPERTY() class ADispareitorHUD* DispareitorHUD;	
	UPROPERTY()	class ADispareitorModoJuego* DispareitorModoJuego;
	float TiempoInicioNivel = 0.f;
	float TiempoPartida = 0.f;
	float TiempoCalentamiento = 0.f;
	float TiempoEnfriamiento = 0.f; 
	uint32 SegundosRestantes = 0;
	UPROPERTY(ReplicatedUsing = AlReplicar_EstadoPartida) FName EstadoPartida;
	UFUNCTION()	void AlReplicar_EstadoPartida();
	UPROPERTY() class UPantallaDelPersonaje* PantallaDelPersonaje;
	bool bPantallaDelPersonajeInicializada = false;
	// Variables cache usadas durante el proceso de Sondeo
	float VidaHUD;
	float VidaMaximaHUD;
	float MuertosHUD;
	int32 MuertesHUD; 
	int32 GranadasActualesHUD;
};
