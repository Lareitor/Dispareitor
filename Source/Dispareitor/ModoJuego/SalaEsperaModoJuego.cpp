#include "SalaEsperaModoJuego.h"
#include "GameFramework/GameStateBase.h"
#include "SubsistemaInstanciaJuego.h"

// AGameMode solo existe en el servidor

void ASalaEsperaModoJuego::PostLogin(APlayerController* ControladorJugador) {
    Super::PostLogin(ControladorJugador);

    int32 NumeroDeJugadores = GameState.Get()->PlayerArray.Num();

    //if (GEngine) {
	//	GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Green, FString(TEXT("Numero de jugadores: " + NumeroDeJugadores)));
	//}

    UGameInstance* InstanciaJuego = GetGameInstance();
    if(InstanciaJuego) {
        USubsistemaInstanciaJuego* SubsistemaInstanciaJuego = InstanciaJuego->GetSubsystem<USubsistemaInstanciaJuego>();
        check(SubsistemaInstanciaJuego); // Si la validacion no es correcta se para el programa

        if(NumeroDeJugadores == SubsistemaInstanciaJuego->NumeroConexionesDeseadas) {
            UWorld* Mundo = GetWorld();
            if(Mundo) {
                bUseSeamlessTravel = true;

                FString ModoJuego = SubsistemaInstanciaJuego->ModoJuegoDeseado;
                if(ModoJuego == "TodosContraTodos") {        
                    Mundo->ServerTravel(FString("/Game/Mapas/TCT_ChateauGuillard?listen"));
                } else if (ModoJuego == "DueloPorEquipos") {
                    Mundo->ServerTravel(FString("/Game/Mapas/DPE_Asia?listen"));
                } else if (ModoJuego == "CapturaLaBandera") {
                    Mundo->ServerTravel(FString("/Game/Mapas/CLB_CubosEstilizados?listen"));
                }
            }
        }
    }    
}	
