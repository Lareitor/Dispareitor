#include "DispareitorModoJuegoEquipos.h"
#include "Dispareitor/EstadoJuego/DispareitorEstadoJuego.h"
#include "Dispareitor/EstadoJugador/DispareitorEstadoJugador.h"
#include "Kismet/GameplayStatics.h"

// Invocado si se une un jugador una vez empezada ya la partida
void ADispareitorModoJuegoEquipos::PostLogin(APlayerController* ControladorNuevoJugador) {
    Super::PostLogin(ControladorNuevoJugador);

    ADispareitorEstadoJuego* DEstadoJuego = Cast<ADispareitorEstadoJuego>(UGameplayStatics::GetGameState(this));
    if(DEstadoJuego) {
        ADispareitorEstadoJugador* DEstadoJugador = ControladorNuevoJugador->GetPlayerState<ADispareitorEstadoJugador>();
        if(DEstadoJugador && DEstadoJugador->ObtenerEquipo() == EEquipo::EE_Ninguno) {
            if(DEstadoJuego->ArrayEstadoJugadoresEquipoAzul.Num() >= DEstadoJuego->ArrayEstadoJugadoresEquipoRojo.Num()) {
                DEstadoJuego->ArrayEstadoJugadoresEquipoRojo.AddUnique(DEstadoJugador);
                DEstadoJugador->ActivarEquipo(EEquipo::EE_Rojo);
            } else {
                DEstadoJuego->ArrayEstadoJugadoresEquipoAzul.AddUnique(DEstadoJugador);
                DEstadoJugador->ActivarEquipo(EEquipo::EE_Azul);
            }
        }
    }
}

void ADispareitorModoJuegoEquipos::HandleMatchHasStarted() {
    Super::HandleMatchHasStarted();

    ADispareitorEstadoJuego* DEstadoJuego = Cast<ADispareitorEstadoJuego>(UGameplayStatics::GetGameState(this));
    if(DEstadoJuego) {
        for(auto EstadoJugador : DEstadoJuego->PlayerArray) {
            ADispareitorEstadoJugador* DEstadoJugador = Cast<ADispareitorEstadoJugador>(EstadoJugador.Get());
            if(DEstadoJugador && DEstadoJugador->ObtenerEquipo() == EEquipo::EE_Ninguno) {
                if(DEstadoJuego->ArrayEstadoJugadoresEquipoAzul.Num() >= DEstadoJuego->ArrayEstadoJugadoresEquipoRojo.Num()) {
                    DEstadoJuego->ArrayEstadoJugadoresEquipoRojo.AddUnique(DEstadoJugador);
                    DEstadoJugador->ActivarEquipo(EEquipo::EE_Rojo);
                } else {
                    DEstadoJuego->ArrayEstadoJugadoresEquipoAzul.AddUnique(DEstadoJugador);
                    DEstadoJugador->ActivarEquipo(EEquipo::EE_Azul);
                }
            }
        }
    }
}

void ADispareitorModoJuegoEquipos::Logout(AController* ControladorJugadorSaliendo) {
    ADispareitorEstadoJuego* DEstadoJuego = Cast<ADispareitorEstadoJuego>(UGameplayStatics::GetGameState(this));
    ADispareitorEstadoJugador* DEstadoJugador = ControladorJugadorSaliendo->GetPlayerState<ADispareitorEstadoJugador>();
    if(DEstadoJuego && DEstadoJugador) {
        if(DEstadoJuego->ArrayEstadoJugadoresEquipoAzul.Contains(DEstadoJugador)) {
            DEstadoJuego->ArrayEstadoJugadoresEquipoAzul.Remove(DEstadoJugador);
        } else {
            DEstadoJuego->ArrayEstadoJugadoresEquipoRojo.Remove(DEstadoJugador);
        }
    }
}

