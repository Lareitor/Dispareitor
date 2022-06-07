#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "DispareitorControladorJugador.generated.h"

UCLASS()
class DISPAREITOR_API ADispareitorControladorJugador : public APlayerController {
	GENERATED_BODY()

public:
	void ActualizarHUDVida(float Vida, float VidaMaxima);
	void ActualizarHUDPuntuacion(float Puntuacion);
	virtual void OnPossess(APawn* Peon) override;
	
protected:
	virtual void BeginPlay() override;

private:
	class ADispareitorHUD* DispareitorHUD;	
	
};
