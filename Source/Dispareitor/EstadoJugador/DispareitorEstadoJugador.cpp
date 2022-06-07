#include "DispareitorEstadoJugador.h"
#include "Dispareitor/Personaje/DispareitorPersonaje.h"
#include "Dispareitor/ControladorJugador/DispareitorControladorJugador.h"

// Este metodo solo se ejecuta en el servidor
void ADispareitorEstadoJugador::IncrementarPuntuacion(float PuntuacionIncremento) {
   SetScore(GetScore() + PuntuacionIncremento); // Score es una variable heredada de APlayerState que se replica 
   DispareitorPersonaje = DispareitorPersonaje == nullptr ? Cast<ADispareitorPersonaje>(GetPawn()) : DispareitorPersonaje;
   if(DispareitorPersonaje) {
      DispareitorControladorJugador = DispareitorControladorJugador == nullptr ? Cast<ADispareitorControladorJugador>(DispareitorPersonaje->Controller) : DispareitorControladorJugador;
      if(DispareitorControladorJugador) {
         UE_LOG(LogTemp, Warning, TEXT("ADispareitorEstadoJugador.IncrementarPuntuacion OK"));
         DispareitorControladorJugador->ActualizarHUDPuntuacion(GetScore());
      }
   }
}

void ADispareitorEstadoJugador::OnRep_Score() {
   Super::OnRep_Score();

   DispareitorPersonaje = DispareitorPersonaje == nullptr ? Cast<ADispareitorPersonaje>(GetPawn()) : DispareitorPersonaje;
   if(DispareitorPersonaje) {
      DispareitorControladorJugador = DispareitorControladorJugador == nullptr ? Cast<ADispareitorControladorJugador>(DispareitorPersonaje->Controller) : DispareitorControladorJugador;
      if(DispareitorControladorJugador) {
         DispareitorControladorJugador->ActualizarHUDPuntuacion(GetScore());
      }
   }
}

