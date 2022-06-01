#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "DispareitorModoJuego.generated.h"

UCLASS()
class DISPAREITOR_API ADispareitorModoJuego : public AGameMode {
	GENERATED_BODY()

public:
	virtual void JugadorEliminado(class ADispareitorPersonaje* VictimaDispareitorJugador, class ADispareitorControladorJugador* VictimaDispareitorControladorJugador, class ADispareitorControladorJugador* AtacanteDispareitorControladorJugador);	
	virtual void PeticionReaparecer(ACharacter* PersonajeEliminado, AController* ControladorEliminado);
};
