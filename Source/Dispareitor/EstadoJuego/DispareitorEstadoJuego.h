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
	UPROPERTY(Replicated) TArray<ADispareitorEstadoJugador*> ArrayDeEstadoJugadoresConPuntuacionMasAlta;

	void ActualizarPuntuacionEquipoAzul();
	void ActualizarPuntuacionEquipoRojo();
	TArray<ADispareitorEstadoJugador*> ArrayEstadoJugadoresEquipoRojo;
	TArray<ADispareitorEstadoJugador*> ArrayEstadoJugadoresEquipoAzul;
	UPROPERTY(ReplicatedUsing = AlReplicar_PuntuacionEquipoRojo) float PuntuacionEquipoRojo = 0.f;
	UPROPERTY(ReplicatedUsing = AlReplicar_PuntuacionEquipoAzul) float PuntuacionEquipoAzul = 0.f;
	UFUNCTION() void AlReplicar_PuntuacionEquipoRojo();
	UFUNCTION() void AlReplicar_PuntuacionEquipoAzul();

private:
	float PuntuacionMasAlta = 0.f;	
};
