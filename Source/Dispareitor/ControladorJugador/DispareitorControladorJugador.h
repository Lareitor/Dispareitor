#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "DispareitorControladorJugador.generated.h"

UCLASS()
class DISPAREITOR_API ADispareitorControladorJugador : public APlayerController {
	GENERATED_BODY()

public:
	void HUDVidaActualizar(float Vida, float VidaMaxima);
	void HUDMuertosActualizar(float Muertos);
	void HUDMuertesActualizar(int32 Muertes);
	void HUDArmaMunicionActualizar(int32 ArmaMunicion);
	void HUDPersonajeMunicionActualizar(int32 PersonajeMunicion);
	void HUDPartidaTiempoActualizar(float _PartidaTiempo);
	void HUDCalentamientoTiempoActualizar(float _CalentamientoTiempo);
	virtual void OnPossess(APawn* Peon) override;
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual float ServidorTiempoObtener();
	virtual void ReceivedPlayer() override;
	void PartidaEstadoActualizar(FName Estado);

	void PartidaEstadoManejador();
	
protected:
	virtual void BeginPlay() override;
	void ActivarHUDTiempo();
	void SondearInicio();

	// Sincronizacion tiempos entre cliente y servidor

	UFUNCTION(Server, Reliable)
	void TiempoServidorPeticion_EnServidor(float TiempoClientePeticion);

	UFUNCTION(Client, Reliable)
	void TiempoServidorDevolucion_EnCliente(float TiempoClientePeticion, float TiempoServidorAlRecibirPeticion);

	float TiempoServidorClienteDelta = 0.f; 

	UPROPERTY(EditAnywhere, Category = Tiempo)
	float TiempoSincronizacionFrecuencia = 5.f;

	float TiempoSincronizacionPasado = 0.f;
	void TiempoSincronizacionComprobar(float DeltaTime);

	UFUNCTION(Server, Reliable)
	void PartidaEstadoComprobar_EnServidor();

	UFUNCTION(Client, Reliable)
	void PartidaEstadoComprobar_EnCliente(FName _PartidaEstado, float _CalentamientoTiempo, float _PartidaTiempo, float _InicioNivelTiempo);

private:
	UPROPERTY()
	class ADispareitorHUD* DispareitorHUD;	

	float InicioNivelTiempo = 0.f;
	float PartidaTiempo = 0.f;
	float CalentamientoTiempo = 0.f;
	uint32 SegundosRestantes = 0;

	UPROPERTY(ReplicatedUsing = PartidaEstado_AlReplicar)
	FName PartidaEstado;

	UFUNCTION()
	void PartidaEstado_AlReplicar();

	UPROPERTY()
	class UPantallaDelPersonaje* PantallaDelPersonaje;
	bool bPantallaDelPersonajeInicializada = false;

	// Variables cache usadas durante el proceso de Sondeo
	float HUDVida;
	float HUDVidaMaxima;
	float HUDMuertos;
	int32 HUDMuertes; 
};
