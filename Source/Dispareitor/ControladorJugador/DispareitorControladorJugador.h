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
	virtual void OnPossess(APawn* Peon) override;
	
protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	class ADispareitorHUD* DispareitorHUD;	
	
};
