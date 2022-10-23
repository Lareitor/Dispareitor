#include "DispareitorEstadoJugador.h"
#include "Dispareitor/Personaje/DispareitorPersonaje.h"
#include "Dispareitor/ControladorJugador/DispareitorControladorJugador.h"
#include "Net/UnrealNetwork.h"

// APlayerState existe en el servidor y en todos los clientes
// Podriamos haber almacenado aqui la municion del jugador, pero dado que se replica menos rapido que el propio jugador se opta por hacerlo en CombateComponente (que pertenece al jugador) 

void ADispareitorEstadoJugador::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const {
   Super::GetLifetimeReplicatedProps(OutLifetimeProps);

   DOREPLIFETIME(ADispareitorEstadoJugador, Muertes);
   DOREPLIFETIME(ADispareitorEstadoJugador, Equipo);
}

// Este metodo solo se ejecuta en el servidor
// Llamado por ADispareitorPersonaje::SondearInicializacion y ADispareitorModoJuego::JugadorEliminado
void ADispareitorEstadoJugador::IncrementarMuertos(float Incremento) {
   SetScore(GetScore() + Incremento); // Score es una variable heredada de APlayerState que se replica 
   DispareitorPersonaje = DispareitorPersonaje != nullptr ? DispareitorPersonaje : Cast<ADispareitorPersonaje>(GetPawn());
   if(DispareitorPersonaje) {
      DispareitorControladorJugador = DispareitorControladorJugador != nullptr ? DispareitorControladorJugador : Cast<ADispareitorControladorJugador>(DispareitorPersonaje->Controller);
      if(DispareitorControladorJugador) {
         DispareitorControladorJugador->ActualizarMuertosHUD(GetScore());
      }
   }
}

void ADispareitorEstadoJugador::OnRep_Score() {
   Super::OnRep_Score();

   DispareitorPersonaje = DispareitorPersonaje != nullptr ? DispareitorPersonaje : Cast<ADispareitorPersonaje>(GetPawn());
   if(DispareitorPersonaje) {
      DispareitorControladorJugador = DispareitorControladorJugador != nullptr ? DispareitorControladorJugador : Cast<ADispareitorControladorJugador>(DispareitorPersonaje->Controller);
      if(DispareitorControladorJugador) {
         DispareitorControladorJugador->ActualizarMuertosHUD(GetScore());
      }
   }
}

// Llamado por ADispareitorPersonaje::SondearInicializacion y ADispareitorModoJuego::JugadorEliminado
void ADispareitorEstadoJugador::IncrementarMuertes(int32 Incremento) {
   Muertes += Incremento;
   DispareitorPersonaje = DispareitorPersonaje != nullptr ? DispareitorPersonaje : Cast<ADispareitorPersonaje>(GetPawn());
   if(DispareitorPersonaje) {
      DispareitorControladorJugador = DispareitorControladorJugador != nullptr ? DispareitorControladorJugador : Cast<ADispareitorControladorJugador>(DispareitorPersonaje->Controller);
      if(DispareitorControladorJugador) {
         DispareitorControladorJugador->ActualizarMuertesHUD(Muertes);
      }
   }
}

void ADispareitorEstadoJugador::AlReplicar_Muertes() {
   DispareitorPersonaje = DispareitorPersonaje != nullptr ? DispareitorPersonaje : Cast<ADispareitorPersonaje>(GetPawn());
   if(DispareitorPersonaje) {
      DispareitorControladorJugador = DispareitorControladorJugador != nullptr ? DispareitorControladorJugador : Cast<ADispareitorControladorJugador>(DispareitorPersonaje->Controller);
      if(DispareitorControladorJugador) {
         DispareitorControladorJugador->ActualizarMuertesHUD(Muertes);
      }
   }
}


void ADispareitorEstadoJugador::ActivarEquipo(EEquipo EquipoAAsignar) {
   Equipo = EquipoAAsignar;
   ADispareitorPersonaje* DPersonaje = Cast<ADispareitorPersonaje>(GetPawn());
   if(DPersonaje) {
      DPersonaje->ActivarColorEquipo(Equipo);
   }
}

void ADispareitorEstadoJugador::AlReplicar_Equipo() {
   ADispareitorPersonaje* DPersonaje = Cast<ADispareitorPersonaje>(GetPawn());
   if(DPersonaje) {
      DPersonaje->ActivarColorEquipo(Equipo);
   }
}