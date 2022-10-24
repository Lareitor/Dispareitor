#include "DispareitorModoJuegoEquipos.h"
#include "Dispareitor/EstadoJuego/DispareitorEstadoJuego.h"
#include "Dispareitor/EstadoJugador/DispareitorEstadoJugador.h"
#include "Dispareitor/ControladorJugador/DispareitorControladorJugador.h"
#include "Kismet/GameplayStatics.h"


ADispareitorModoJuegoEquipos::ADispareitorModoJuegoEquipos() {    
    bPartidaPorEquipos = true;
}

// Invocado si se une un jugador una vez empezada ya la partida
void ADispareitorModoJuegoEquipos::PostLogin(APlayerController* CNuevoJugador) {
    Super::PostLogin(CNuevoJugador);

    ADispareitorEstadoJuego* DEstadoJuego = Cast<ADispareitorEstadoJuego>(UGameplayStatics::GetGameState(this));
    if(DEstadoJuego) {
        ADispareitorEstadoJugador* DEstadoJugador = CNuevoJugador->GetPlayerState<ADispareitorEstadoJugador>();
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

void ADispareitorModoJuegoEquipos::Logout(AController* CJugadorSaliendo) {
    ADispareitorEstadoJuego* DEstadoJuego = Cast<ADispareitorEstadoJuego>(UGameplayStatics::GetGameState(this));
    ADispareitorEstadoJugador* DEstadoJugador = CJugadorSaliendo->GetPlayerState<ADispareitorEstadoJugador>();
    if(DEstadoJuego && DEstadoJugador) {
        if(DEstadoJuego->ArrayEstadoJugadoresEquipoAzul.Contains(DEstadoJugador)) {
            DEstadoJuego->ArrayEstadoJugadoresEquipoAzul.Remove(DEstadoJugador);
        } else {
            DEstadoJuego->ArrayEstadoJugadoresEquipoRojo.Remove(DEstadoJugador);
        }
    }
}

float ADispareitorModoJuegoEquipos::CalcularDanio(AController* CAtacante, AController* CVictima, float Danio) {
    ADispareitorEstadoJugador* DEstadoJugadorAtacante = CAtacante->GetPlayerState<ADispareitorEstadoJugador>();
    ADispareitorEstadoJugador* DEstadoJugadorVictima = CVictima->GetPlayerState<ADispareitorEstadoJugador>();
    if((!DEstadoJugadorAtacante || !DEstadoJugadorVictima) || DEstadoJugadorAtacante == DEstadoJugadorVictima || DEstadoJugadorAtacante->ObtenerEquipo() != DEstadoJugadorVictima->ObtenerEquipo()) {
        return Danio;
    }
   
    return 0.f; // Son jugadores del mismo equipo
}

void ADispareitorModoJuegoEquipos::JugadorEliminado(ADispareitorPersonaje* DPersonajeVictima, ADispareitorControladorJugador* DControladorJugadorVictima, ADispareitorControladorJugador* DControladorJugadorAtacante) {
    Super::JugadorEliminado(DPersonajeVictima, DControladorJugadorVictima,  DControladorJugadorAtacante);

    ADispareitorEstadoJuego* DEstadoJuego = Cast<ADispareitorEstadoJuego>(UGameplayStatics::GetGameState(this));
    ADispareitorEstadoJugador* DEstadoJugador = DControladorJugadorAtacante ? Cast<ADispareitorEstadoJugador>(DControladorJugadorAtacante->PlayerState) : nullptr;
    if(DEstadoJuego && DEstadoJugador){
        if(DEstadoJugador->ObtenerEquipo() == EEquipo::EE_Azul) {
            DEstadoJuego->ActualizarPuntuacionEquipoAzul();
        } else if(DEstadoJugador->ObtenerEquipo() == EEquipo::EE_Rojo) {
            DEstadoJuego->ActualizarPuntuacionEquipoRojo();
        }

    }
}	
