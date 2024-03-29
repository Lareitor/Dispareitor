#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "DispareitorControladorJugador.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDelegadoPingAlto, bool, bPingAlto);

UCLASS()
class DISPAREITOR_API ADispareitorControladorJugador : public APlayerController {
	GENERATED_BODY()

public:
	void ActualizarVidaHUD(float Vida, float VidaMaxima);
	void ActualizarEscudoHUD(float Escudo, float EscudoMaximo);
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
	void ActualizarEstadoPartida(FName Estado, bool bPartidaPorEquipos = false);
	void ManejarEstadoPartidaHaEmpezado(bool bPartidaPorEquipos = false);
	void ManejarEstadoPartidaEnfriamiento();
	float STT = 0.f; // Single Trip Time es la mitad aprox. de RTT
	FDelegadoPingAlto DelegadoPingAlto;
	
	void AnunciarEliminacion(APlayerState* EstadoJugadorGanador, APlayerState* EstadoJugadorPerdedor);
	void EsconderPuntuacionEquipos();
	void InicializarPuntuacionEquipos();
	void ActualizarPuntuacionEquipoRojoHUD(int32 Puntuacion);
	void ActualizarPuntuacionEquipoAzulHUD(int32 Puntuacion);

	
protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override; 
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
	
	void IniciarAnimacionPingAlto();
	void PararAnimacionPingAlto();
	void ComprobarPingAlto(float DeltaTime);

	void MostrarRegresarAMenuPrincipal();

	UFUNCTION(Client, Reliable) void AnunciarEliminacion_EnCliente(APlayerState* EstadoJugadorGanador, APlayerState* EstadoJugadorPerdedor);

	UPROPERTY(ReplicatedUsing = AlReplicar_MostrarPuntuacionEquipos) bool bMostrarPuntuacionEquipos = false;
	UFUNCTION() void AlReplicar_MostrarPuntuacionEquipos();

	FString ObtenerAnuncio(const TArray<class ADispareitorEstadoJugador*>& DEstadoJugadores);
	FString ObtenerAnuncioEquipos(class ADispareitorEstadoJuego* DEstadoJuego);

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
	// Variables cache usadas durante el proceso de Sondeo
	float VidaHUD;
	bool bInicializadaVida = false;
	float VidaMaximaHUD;
	float EscudoHUD;
	bool bInicializadoEscudo = false;
	float EscudoMaximoHUD;
	float MuertosHUD;
	bool bInicializadoMuertos = false;
	int32 MuertesHUD; 
	bool bInicializadoMuertes = false;
	int32 GranadasActualesHUD;
	bool bInicializadaGranadas = false;
	float MunicionPersonajeHUD;
	bool bInicializadaMunicionPersonaje = false;
	float MunicionArmaHUD;
	bool bInicializadaMunicionArma = false;
	bool bSondearInicio = false;

	float TiempoParaSiguienteComprobacionPingAlto = 0.f;
	float TiempoEjecutandoseAnimacionPingAlto = 0.f;
	UPROPERTY(EditAnywhere) float DuracionAnimacionPingAlto = 5.f;
	UPROPERTY(EditAnywhere) float FrecuenciaChequeoPingAlto = 20.f;
	UPROPERTY(EditAnywhere) float UmbralPingAlto = 50.f;
	UFUNCTION(Server, Reliable) void ReportarEstadoPing_EnServidor(bool bPingAlto);

	UPROPERTY(EditAnywhere, Category = HUD) TSubclassOf<class UUserWidget> ClaseRegresarAMenuPrincipal;
	UPROPERTY() class URegresarAMenuPrincipal* RegresarAMenuPrincipal;
	bool bRegresarAMenuPrincipal = false;
};
