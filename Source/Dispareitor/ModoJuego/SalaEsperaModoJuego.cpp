#include "SalaEsperaModoJuego.h"
#include "GameFramework/GameStateBase.h"

void ASalaEsperaModoJuego::PostLogin(APlayerController* ControladorJugador) {
    Super::PostLogin(ControladorJugador);

    int32 NumeroDeJugadores = GameState.Get()->PlayerArray.Num();

    if (GEngine) {
		GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Green, FString(TEXT("Numero de jugadores: " + NumeroDeJugadores)));
	}

    if(NumeroDeJugadores == 3) {
        UWorld* Mundo = GetWorld();
        if(Mundo) {
            bUseSeamlessTravel = true;
            Mundo->ServerTravel(FString("/Game/Mapas/Dispareitor?listen"));
        }
    }
}	
