#pragma once

#include "CoreMinimal.h"
#include "DispareitorModoJuego.h"
#include "DispareitorModoJuegoEquipos.generated.h"

UCLASS()
class DISPAREITOR_API ADispareitorModoJuegoEquipos : public ADispareitorModoJuego {
	GENERATED_BODY()

public :	
	virtual void PostLogin(APlayerController* ControladorNuevoJugador) override;
	virtual void Logout(AController* ControladorJugadorSaliendo) override;

protected:	
	virtual void HandleMatchHasStarted() override;
	
};
