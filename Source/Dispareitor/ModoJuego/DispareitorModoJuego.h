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
	virtual void JugadorEliminado(class ADispareitorPersonaje* DispareitorPersonajeVictima, class ADispareitorControladorJugador* DispareitorControladorJugadorVictima, class ADispareitorControladorJugador* DispareitorControladorJugadorAtacante);	
	virtual void PeticionReaparecer(ACharacter* PersonajeEliminado, AController* ControladorEliminado);
	void JugadorDejaJuego(class ADispareitorEstadoJugador* DispareitorEstadoJugador);
	UPROPERTY(EditDefaultsOnly)	float TiempoCalentamiento = 10.f;
	UPROPERTY(EditDefaultsOnly)	float TiempoPartida = 300.f;
	UPROPERTY(EditDefaultsOnly)	float TiempoEnfriamiento = 10.f;
	float TiempoInicioNivel = 0.f;
	virtual float CalcularDanio(AController* CAtacante, AController* CVictima, float Danio);
	bool bPartidaPorEquipos = false;
	void SituarArmaTrasCaerEnLimitesJuego(class AArma* Arma);
	void ActualizarPuntoReaparicionArmaALibre(FString NombrePuntoReaparicion);

protected:
	virtual void BeginPlay() override;	
	virtual void OnMatchStateSet() override;

private:
	float TiempoCuentaAtras = 0.f;
	void SituarArmas();
	UPROPERTY() TMap<FString, bool> MapaNombrePuntoReaparicionArmaOcupado;
	UPROPERTY()	TArray<AActor*> PuntosReaparicionArmas; 

public:
	FORCEINLINE float ObtenerTiempoCuentaAtras() const { return TiempoCuentaAtras; }	
};
