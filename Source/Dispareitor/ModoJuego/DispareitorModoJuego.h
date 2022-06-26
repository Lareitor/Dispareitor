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
	float CalentamientoTiempo = 10.f;

	UPROPERTY(EditDefaultsOnly)
	float PartidaTiempo = 120.f;

	float InicioNivelTiempo = 0.f;

protected:
	virtual void BeginPlay() override;	
	virtual void OnMatchStateSet() override;

private:
	float CalentamientoTiempoRestante = 0.f;
};
