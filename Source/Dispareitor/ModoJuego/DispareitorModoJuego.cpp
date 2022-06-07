#include "DispareitorModoJuego.h"
#include "Dispareitor/Personaje/DispareitorPersonaje.h"
#include "Dispareitor/ControladorJugador/DispareitorControladorJugador.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"
#include "Dispareitor/EstadoJugador/DispareitorEstadoJugador.h"

// Llamado por ADispareitorPersonaje::RecibirDano
void ADispareitorModoJuego::JugadorEliminado(class ADispareitorPersonaje* VictimaDispareitorJugador, class ADispareitorControladorJugador* VictimaDispareitorControladorJugador, class ADispareitorControladorJugador* AtacanteDispareitorControladorJugador) {
    ADispareitorEstadoJugador* AtacanteEstadoJugador = AtacanteDispareitorControladorJugador ? Cast<ADispareitorEstadoJugador>(AtacanteDispareitorControladorJugador->PlayerState) : nullptr;
    ADispareitorEstadoJugador* VictimaEstadoJugador = VictimaDispareitorControladorJugador ? Cast<ADispareitorEstadoJugador>(VictimaDispareitorControladorJugador->PlayerState) : nullptr;

    if(AtacanteEstadoJugador && AtacanteEstadoJugador != VictimaEstadoJugador) {
        AtacanteEstadoJugador->IncrementarPuntuacion(100.f);
    }

    if(VictimaDispareitorJugador) {
        VictimaDispareitorJugador->Eliminado();
    }
}

// Llamado por ADispareitorPersonaje::TemporizadorEliminadoFinalizado
void ADispareitorModoJuego::PeticionReaparecer(ACharacter* PersonajeEliminado, AController* ControladorEliminado) {
    if(PersonajeEliminado) {
        PersonajeEliminado->Reset(); // Desune el controlador del caracter
        PersonajeEliminado->Destroy(); // Destruye al caracter pero el controlador sobrevive asi como otras clases como player state
    }    

    if(ControladorEliminado) {
        TArray<AActor*> IniciosDeJugador;
        UGameplayStatics::GetAllActorsOfClass(this, APlayerStart::StaticClass(), IniciosDeJugador);
        int32 IndiceIniciosDeJugadorElegido = FMath::RandRange(0, IniciosDeJugador.Num() - 1);
        
        RestartPlayerAtPlayerStart(ControladorEliminado, IniciosDeJugador[IndiceIniciosDeJugadorElegido]);
    }
}
