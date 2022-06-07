#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "DispareitorEstadoJugador.generated.h"

UCLASS()
class DISPAREITOR_API ADispareitorEstadoJugador : public APlayerState {
	GENERATED_BODY()

public:
	virtual void OnRep_Score() override;
	void IncrementarPuntuacion(float PuntuacionIncremento);

private:	
	class ADispareitorPersonaje* DispareitorPersonaje;
	class ADispareitorControladorJugador* DispareitorControladorJugador;	
};
