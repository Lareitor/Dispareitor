#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "SalaEsperaModoJuego.generated.h"

UCLASS()
class DISPAREITOR_API ASalaEsperaModoJuego : public AGameMode {
	GENERATED_BODY()

public:
	virtual void PostLogin(APlayerController* ControladorJugador) override;		
};
