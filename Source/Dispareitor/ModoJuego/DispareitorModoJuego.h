#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "DispareitorModoJuego.generated.h"

namespace MatchState {
	extern DISPAREITOR_API const FName Enfriamiento; 
}

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

	UPROPERTY(EditDefaultsOnly)
	float EnfriamientoTiempo = 10.f;

	float InicioNivelTiempo = 0.f;

protected:
	virtual void BeginPlay() override;	
	virtual void OnMatchStateSet() override;

private:
	float CuentaAtrasTiempo = 0.f;

	void ArmasSituar();

public:
	FORCEINLINE float CuentaAtrasTiempoObtener() const { return CuentaAtrasTiempo; }	
};
