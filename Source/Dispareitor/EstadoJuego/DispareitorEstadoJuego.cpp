#include "DispareitorEstadoJuego.h"
#include "Net/UnrealNetwork.h"
#include "Dispareitor/EstadoJugador/DispareitorEstadoJugador.h"

// AGameState existe en el servidor y en los clientes 

void ADispareitorEstadoJuego::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(ADispareitorEstadoJuego, EstadoJugadoresPuntuacionMasAlta);
}

// Llamado por ADispareitorModoJuego::JugadorEliminado
void ADispareitorEstadoJuego::EstadoJugadoresPuntuacionMasAltaActualizar(class ADispareitorEstadoJugador* DispareitorEstadoJugador) {
    if(EstadoJugadoresPuntuacionMasAlta.Num() == 0) {
        EstadoJugadoresPuntuacionMasAlta.Add(DispareitorEstadoJugador);
        PuntuacionMasAlta = DispareitorEstadoJugador->GetScore();
    } else if(DispareitorEstadoJugador->GetScore() == PuntuacionMasAlta) {
        EstadoJugadoresPuntuacionMasAlta.AddUnique(DispareitorEstadoJugador);
    } else if(DispareitorEstadoJugador->GetScore() > PuntuacionMasAlta) {
        EstadoJugadoresPuntuacionMasAlta.Empty();
        EstadoJugadoresPuntuacionMasAlta.AddUnique(DispareitorEstadoJugador);
        PuntuacionMasAlta = DispareitorEstadoJugador->GetScore();
    }  
}


