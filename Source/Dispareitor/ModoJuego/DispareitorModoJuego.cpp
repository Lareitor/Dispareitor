#include "DispareitorModoJuego.h"
#include "Dispareitor/Personaje/DispareitorPersonaje.h"
#include "Dispareitor/ControladorJugador/DispareitorControladorJugador.h"

// Llamado por ADispareitorPersonaje::RecibirDano
void ADispareitorModoJuego::JugadorEliminado(class ADispareitorPersonaje* VictimaDispareitorJugador, class ADispareitorControladorJugador* VictimaDispareitorControladorJugador, class ADispareitorControladorJugador* AtacanteDispareitorControladorJugador) {
    if(VictimaDispareitorJugador) {
        VictimaDispareitorJugador->MulticastEliminado();
    }
}
