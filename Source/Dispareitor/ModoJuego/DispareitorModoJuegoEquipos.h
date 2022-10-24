#pragma once

#include "CoreMinimal.h"
#include "DispareitorModoJuego.h"
#include "DispareitorModoJuegoEquipos.generated.h"

UCLASS()
class DISPAREITOR_API ADispareitorModoJuegoEquipos : public ADispareitorModoJuego {
	GENERATED_BODY()

public :	
	ADispareitorModoJuegoEquipos();
	virtual void PostLogin(APlayerController* CNuevoJugador) override;
	virtual void Logout(AController* CJugadorSaliendo) override;
	virtual float CalcularDanio(AController* CAtacante, AController* CVictima, float Danio) override;
	virtual void JugadorEliminado(class ADispareitorPersonaje* DPersonajeVictima, class ADispareitorControladorJugador* DControladorJugadorVictima, class ADispareitorControladorJugador* DControladorJugadorAtacante) override;	

protected:	
	virtual void HandleMatchHasStarted() override;
	
};
