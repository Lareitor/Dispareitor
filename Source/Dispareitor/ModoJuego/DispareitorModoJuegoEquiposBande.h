#pragma once

#include "CoreMinimal.h"
#include "DispareitorModoJuegoEquipos.h"
#include "DispareitorModoJuegoEquiposBande.generated.h"

// No permite nombres de clases de más de 32 caracteres :-/
UCLASS()
class DISPAREITOR_API ADispareitorModoJuegoEquiposBande : public ADispareitorModoJuegoEquipos {
	GENERATED_BODY()	
	
public:
	virtual void JugadorEliminado(class ADispareitorPersonaje* DPersonajeVictima, class ADispareitorControladorJugador* DControladorJugadorVictima, class ADispareitorControladorJugador* DControladorJugadorAtacante) override;	
	void BanderaCapturada(class AArmaBandera* ArmaBandera, class AAreaBandera*  AreaBandera);
};
