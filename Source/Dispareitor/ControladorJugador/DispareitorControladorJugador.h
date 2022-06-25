#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "DispareitorControladorJugador.generated.h"

UCLASS()
class DISPAREITOR_API ADispareitorControladorJugador : public APlayerController {
	GENERATED_BODY()

public:
	void ActualizarHUDVida(float Vida, float VidaMaxima);
	void ActualizarHUDMuertos(float Muertos);
	void ActualizarHUDMuertes(int32 Muertes);
	void ActualizarHUDMunicionArma(int32 MunicionArma);
	void ActualizarHUDMunicionPersonaje(int32 MunicionPersonaje);
	void ActualizarHUDTiempo(float Tiempo);
	virtual void OnPossess(APawn* Peon) override;
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual float TiempoServidorObtener();
	virtual void ReceivedPlayer() override;
	void PartidaEstadoActualizar(FName Estado);

	void PartidaEstadoManejador();
	
protected:
	virtual void BeginPlay() override;
	void ActivarHUDTiempo();
	void SondearInicio();

	// Sincronizacion tiempos entre cliente y servidor

	UFUNCTION(Server, Reliable)
	void TiempoServidorPeticion(float TiempoClientePeticion);

	UFUNCTION(Client, Reliable)
	void TiempoServidorDevolucion(float TiempoClientePeticion, float TiempoServidorAlRecibirPeticion);

	float TiempoServidorClienteDelta = 0.f; 

	UPROPERTY(EditAnywhere, Category = Tiempo)
	float TiempoSincronizacionFrecuencia = 5.f;

	float TiempoSincronizacionPasado = 0.f;
	void TiempoSincronizacionComprobar(float DeltaTime);


private:
	UPROPERTY()
	class ADispareitorHUD* DispareitorHUD;	

	float PartidaTiempo = 120.f;
	uint32 SegundosRestantes = 0;

	UPROPERTY(ReplicatedUsing = PartidaEstadoAlReplicar)
	FName PartidaEstado;

	UFUNCTION()
	void PartidaEstadoAlReplicar();

	UPROPERTY()
	class UPantallaDelPersonaje* PantallaDelPersonaje;
	bool bPantallaDelPersonajeInicializada = false;

	// Variables cache usadas durante el proceso de Sondeo
	float HUDVida;
	float HUDVidaMaxima;
	float HUDMuertos;
	int32 HUDMuertes; 
};
