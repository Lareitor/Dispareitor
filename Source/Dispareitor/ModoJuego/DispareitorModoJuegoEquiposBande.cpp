#include "DispareitorModoJuegoEquiposBande.h"
#include "Dispareitor/Arma/ArmaBandera.h"
#include "Dispareitor/CapturaBandera/AreaBandera.h"
#include "Dispareitor/EstadoJuego/DispareitorEstadoJuego.h"

void ADispareitorModoJuegoEquiposBande::JugadorEliminado(class ADispareitorPersonaje* DPersonajeVictima, class ADispareitorControladorJugador* DControladorJugadorVictima, class ADispareitorControladorJugador* DControladorJugadorAtacante) {
    ADispareitorModoJuego::JugadorEliminado(DPersonajeVictima, DControladorJugadorVictima,  DControladorJugadorAtacante);
}

void ADispareitorModoJuegoEquiposBande::BanderaCapturada(class AArmaBandera* ArmaBandera, class AAreaBandera*  AreaBandera) {
    bool bCapturaValida = ArmaBandera->ObtenerEquipo() != AreaBandera->Equipo;
    ADispareitorEstadoJuego* DEstadoJuego = Cast<ADispareitorEstadoJuego>(GameState);
    if(DEstadoJuego) {
        if(AreaBandera->Equipo == EEquipo::EE_Azul) {
            DEstadoJuego->ActualizarPuntuacionEquipoAzul();
        }
        if(AreaBandera->Equipo == EEquipo::EE_Rojo) {
            DEstadoJuego->ActualizarPuntuacionEquipoRojo();
        }
    }
}



