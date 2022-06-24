#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "DispareitorModoJuego.generated.h"

UCLASS()
class DISPAREITOR_API ADispareitorModoJuego : public AGameMode {
	GENERATED_BODY()

public:
	ADispareitorModoJuego();
	virtual void Tick(float DeltaTime) override;
	virtual void JugadorEliminado(class ADispareitorPersonaje* VictimaDispareitorJugador, class ADispareitorControladorJugador* VictimaDispareitorControladorJugador, class ADispareitorControladorJugador* AtacanteDispareitorControladorJugador);	
	virtual void PeticionReaparecer(ACharacter* PersonajeEliminado, AController* ControladorEliminado);

	UPROPERTY(EditDefaultsOnly)
	float TiempoCalentamiento = 10.f;

	float TiempoInicioNivel = 0.f;

protected:
	virtual void BeginPlay() override;	
	virtual void OnMatchStateSet() override;

private:
	float TiempoCalentamientoRestante = 0.f;
};
