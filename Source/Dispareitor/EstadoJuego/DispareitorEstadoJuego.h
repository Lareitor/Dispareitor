#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "DispareitorEstadoJuego.generated.h"

UCLASS()
class DISPAREITOR_API ADispareitorEstadoJuego : public AGameState {
	GENERATED_BODY()

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	void ActualizarArrayDeEstadoJugadoresConPuntuacionMasAlta(class ADispareitorEstadoJugador* DispareitorEstadoJugador);

	UPROPERTY(Replicated)
	TArray<ADispareitorEstadoJugador*> ArrayDeEstadoJugadoresConPuntuacionMasAlta;

private:
	float PuntuacionMasAlta = 0.f;	
};
