#pragma once

#include "CoreMinimal.h"
#include "DispareitorModoJuego.h"
#include "DispareitorModoJuegoEquipos.generated.h"

UCLASS()
class DISPAREITOR_API ADispareitorModoJuegoEquipos : public ADispareitorModoJuego {
	GENERATED_BODY()

public :	
	virtual void PostLogin(APlayerController* CNuevoJugador) override;
	virtual void Logout(AController* CJugadorSaliendo) override;
	virtual float CalcularDanio(AController* CAtacante, AController* CVictima, float Danio) override;

protected:	
	virtual void HandleMatchHasStarted() override;
	
};
