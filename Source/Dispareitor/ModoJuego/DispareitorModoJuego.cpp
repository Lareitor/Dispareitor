#include "DispareitorModoJuego.h"
#include "Dispareitor/Personaje/DispareitorPersonaje.h"
#include "Dispareitor/ControladorJugador/DispareitorControladorJugador.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"
#include "Dispareitor/EstadoJugador/DispareitorEstadoJugador.h"

// AGameMode solo existe en el servidor

ADispareitorModoJuego::ADispareitorModoJuego() {
    // Le indicamos que queremos permanecer en el estado WaitingToStart
    // En este estado no tenemos mallas de los personajes y pueden volar libremente por el nivel
    // Activarlo tambien en BP_DispareitorModoJuego
    bDelayedStart = true;
}

void ADispareitorModoJuego::BeginPlay() {
    Super::BeginPlay();

    InicioNivelTiempo = GetWorld()->GetTimeSeconds();
}

void ADispareitorModoJuego::Tick(float DeltaTime) {
    Super::Tick(DeltaTime);

    if(MatchState == MatchState::WaitingToStart) {
        CalentamientoTiempoRestante = CalentamientoTiempo - GetWorld()->GetTimeSeconds() + InicioNivelTiempo;
        if(CalentamientoTiempoRestante <= 0.f) {
            // Pasamos del estado WaitingToStart a InProgress donde los jugadores aparecen ya con las mallas
            StartMatch();
        }
    }
}

void ADispareitorModoJuego::OnMatchStateSet() {
    Super::OnMatchStateSet();

    for(FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It) {
        ADispareitorControladorJugador* DispareitorControladorJugador = Cast<ADispareitorControladorJugador>(*It);
        if(DispareitorControladorJugador) {
            DispareitorControladorJugador->PartidaEstadoActualizar(MatchState);
        }
    }
}

// Llamado por ADispareitorPersonaje::RecibirDano
void ADispareitorModoJuego::JugadorEliminado(class ADispareitorPersonaje* VictimaDispareitorJugador, class ADispareitorControladorJugador* VictimaDispareitorControladorJugador, class ADispareitorControladorJugador* AtacanteDispareitorControladorJugador) {
    ADispareitorEstadoJugador* AtacanteEstadoJugador = AtacanteDispareitorControladorJugador ? Cast<ADispareitorEstadoJugador>(AtacanteDispareitorControladorJugador->PlayerState) : nullptr;
    ADispareitorEstadoJugador* VictimaEstadoJugador = VictimaDispareitorControladorJugador ? Cast<ADispareitorEstadoJugador>(VictimaDispareitorControladorJugador->PlayerState) : nullptr;

    if(AtacanteEstadoJugador && AtacanteEstadoJugador != VictimaEstadoJugador) {
        AtacanteEstadoJugador->IncrementarMuertos(1.f);
    }
    if(VictimaEstadoJugador) {
        VictimaEstadoJugador->IncrementarMuertes(1);
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
