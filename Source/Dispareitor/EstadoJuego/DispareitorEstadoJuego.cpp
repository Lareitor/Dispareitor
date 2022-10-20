#include "DispareitorEstadoJuego.h"
#include "Net/UnrealNetwork.h"
#include "Dispareitor/EstadoJugador/DispareitorEstadoJugador.h"

// AGameState existe en el servidor y en los clientes 

void ADispareitorEstadoJuego::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(ADispareitorEstadoJuego, ArrayDeEstadoJugadoresConPuntuacionMasAlta);
    DOREPLIFETIME(ADispareitorEstadoJuego, PuntuacionEquipoRojo);
    DOREPLIFETIME(ADispareitorEstadoJuego, PuntuacionEquipoAzul);
}

// Llamado por ADispareitorModoJuego::JugadorEliminado
void ADispareitorEstadoJuego::ActualizarArrayDeEstadoJugadoresConPuntuacionMasAlta(class ADispareitorEstadoJugador* DispareitorEstadoJugador) {
    if(ArrayDeEstadoJugadoresConPuntuacionMasAlta.Num() == 0) {
        ArrayDeEstadoJugadoresConPuntuacionMasAlta.Add(DispareitorEstadoJugador);
        PuntuacionMasAlta = DispareitorEstadoJugador->GetScore();
    } else if(DispareitorEstadoJugador->GetScore() == PuntuacionMasAlta) {
        ArrayDeEstadoJugadoresConPuntuacionMasAlta.AddUnique(DispareitorEstadoJugador);
    } else if(DispareitorEstadoJugador->GetScore() > PuntuacionMasAlta) {
        ArrayDeEstadoJugadoresConPuntuacionMasAlta.Empty();
        ArrayDeEstadoJugadoresConPuntuacionMasAlta.AddUnique(DispareitorEstadoJugador);
        PuntuacionMasAlta = DispareitorEstadoJugador->GetScore();
    }  
}

void ADispareitorEstadoJuego::AlReplicar_PuntuacionEquipoRojo() {

}

void ADispareitorEstadoJuego::AlReplicar_PuntuacionEquipoAzul() {

}
