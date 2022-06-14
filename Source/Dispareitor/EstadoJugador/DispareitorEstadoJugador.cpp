#include "DispareitorEstadoJugador.h"
#include "Dispareitor/Personaje/DispareitorPersonaje.h"
#include "Dispareitor/ControladorJugador/DispareitorControladorJugador.h"
#include "Net/UnrealNetwork.h"

// APlayerState existe en el servidor y en todos los clientes
// Podriamos haber almacenado aqui la municion del jugador, pero dado que se replica menos rapido que el propio jugador se opta por hacerlo en CombateComponente (que pertenece al jugador) 

void ADispareitorEstadoJugador::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const {
   Super::GetLifetimeReplicatedProps(OutLifetimeProps);

   DOREPLIFETIME(ADispareitorEstadoJugador, Muertes);
}

// Este metodo solo se ejecuta en el servidor
// Llamado por ADispareitorPersonaje::SondearInicializacion y ADispareitorModoJuego::JugadorEliminado
void ADispareitorEstadoJugador::IncrementarMuertos(float MuertosIncremento) {
   SetScore(GetScore() + MuertosIncremento); // Score es una variable heredada de APlayerState que se replica 
   DispareitorPersonaje = DispareitorPersonaje == nullptr ? Cast<ADispareitorPersonaje>(GetPawn()) : DispareitorPersonaje;
   if(DispareitorPersonaje) {
      DispareitorControladorJugador = DispareitorControladorJugador == nullptr ? Cast<ADispareitorControladorJugador>(DispareitorPersonaje->Controller) : DispareitorControladorJugador;
      if(DispareitorControladorJugador) {
         DispareitorControladorJugador->ActualizarHUDMuertos(GetScore());
      }
   }
}

void ADispareitorEstadoJugador::OnRep_Score() {
   Super::OnRep_Score();

   DispareitorPersonaje = DispareitorPersonaje == nullptr ? Cast<ADispareitorPersonaje>(GetPawn()) : DispareitorPersonaje;
   if(DispareitorPersonaje) {
      DispareitorControladorJugador = DispareitorControladorJugador == nullptr ? Cast<ADispareitorControladorJugador>(DispareitorPersonaje->Controller) : DispareitorControladorJugador;
      if(DispareitorControladorJugador) {
         DispareitorControladorJugador->ActualizarHUDMuertos(GetScore());
      }
   }
}

// Llamado por ADispareitorPersonaje::SondearInicializacion y ADispareitorModoJuego::JugadorEliminado
void ADispareitorEstadoJugador::IncrementarMuertes(int32 MuertesIncremento) {
   Muertes += MuertesIncremento;
   DispareitorPersonaje = DispareitorPersonaje == nullptr ? Cast<ADispareitorPersonaje>(GetPawn()) : DispareitorPersonaje;
   if(DispareitorPersonaje) {
      DispareitorControladorJugador = DispareitorControladorJugador == nullptr ? Cast<ADispareitorControladorJugador>(DispareitorPersonaje->Controller) : DispareitorControladorJugador;
      if(DispareitorControladorJugador) {
         DispareitorControladorJugador->ActualizarHUDMuertes(Muertes);
      }
   }
}

void ADispareitorEstadoJugador::AlReplicarMuertes() {
   DispareitorPersonaje = DispareitorPersonaje == nullptr ? Cast<ADispareitorPersonaje>(GetPawn()) : DispareitorPersonaje;
   if(DispareitorPersonaje) {
      DispareitorControladorJugador = DispareitorControladorJugador == nullptr ? Cast<ADispareitorControladorJugador>(DispareitorPersonaje->Controller) : DispareitorControladorJugador;
      if(DispareitorControladorJugador) {
         DispareitorControladorJugador->ActualizarHUDMuertes(Muertes);
      }
   }
}

