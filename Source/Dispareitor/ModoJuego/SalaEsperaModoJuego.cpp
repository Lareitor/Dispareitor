#include "SalaEsperaModoJuego.h"
#include "GameFramework/GameStateBase.h"

void ASalaEsperaModoJuego::PostLogin(APlayerController* ControladorJugador) {
    Super::PostLogin(ControladorJugador);

    int32 NumeroDeJugadores = GameState.Get()-> PlayerArray.Num();
    if(NumeroDeJugadores == 1) {
        UWorld* Mundo = GetWorld();
        if(Mundo) {
            bUseSeamlessTravel = true;
            Mundo->ServerTravel(FString("/Game/Mapas/Dispareitor?listen"));
        }
    }
}	
