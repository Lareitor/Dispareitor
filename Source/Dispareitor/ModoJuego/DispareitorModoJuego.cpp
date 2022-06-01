#include "DispareitorModoJuego.h"
#include "Dispareitor/Personaje/DispareitorPersonaje.h"
#include "Dispareitor/ControladorJugador/DispareitorControladorJugador.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"

// Llamado por ADispareitorPersonaje::RecibirDano
void ADispareitorModoJuego::JugadorEliminado(class ADispareitorPersonaje* VictimaDispareitorJugador, class ADispareitorControladorJugador* VictimaDispareitorControladorJugador, class ADispareitorControladorJugador* AtacanteDispareitorControladorJugador) {
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
