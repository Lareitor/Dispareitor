#include "DispareitorModoJuego.h"
#include "Dispareitor/Personaje/DispareitorPersonaje.h"
#include "Dispareitor/ControladorJugador/DispareitorControladorJugador.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"
#include "Dispareitor/EstadoJugador/DispareitorEstadoJugador.h"
#include "Dispareitor/EstadoJuego/DispareitorEstadoJuego.h"
#include "Math/NumericLimits.h"
#include "Dispareitor/Arma/Arma.h"


// AGameMode solo existe en el servidor

namespace MatchState {
	const FName Enfriamiento = FName("Enfriamiento"); // Se ha acabado la partida, mostrar ganador y tiempo de enfriamiento (para empezar una nueva)
}

ADispareitorModoJuego::ADispareitorModoJuego() {
    // Le indicamos que queremos permanecer en el estado WaitingToStart
    // En este estado no tenemos mallas de los personajes y pueden volar libremente por el nivel
    // Activarlo tambien en BP_DispareitorModoJuego
    bDelayedStart = true;    
}

void ADispareitorModoJuego::BeginPlay() {
    Super::BeginPlay();
    SituarArmas();
    TiempoInicioNivel = GetWorld()->GetTimeSeconds();
}

void ADispareitorModoJuego::Tick(float DeltaTime) {
    Super::Tick(DeltaTime);

    if(MatchState == MatchState::WaitingToStart) {
        TiempoCuentaAtras = TiempoCalentamiento - GetWorld()->GetTimeSeconds() + TiempoInicioNivel;
        if(TiempoCuentaAtras <= 0.f) {
            // Pasamos del estado WaitingToStart a InProgress donde los jugadores aparecen ya con las mallas
            StartMatch();
        }
    } else if(MatchState == MatchState::InProgress) {
        TiempoCuentaAtras = TiempoCalentamiento + TiempoPartida - GetWorld()->GetTimeSeconds() + TiempoInicioNivel;
        if(TiempoCuentaAtras <= 0.f) {
            SetMatchState(MatchState::Enfriamiento);
        }
    } else if(MatchState == MatchState::Enfriamiento) {
        TiempoCuentaAtras = TiempoCalentamiento + TiempoPartida + TiempoEnfriamiento - GetWorld()->GetTimeSeconds() + TiempoInicioNivel;
        if(TiempoCuentaAtras <= 0.f) {
            RestartGame();
        }
    }
}

// Llamado cada vez que se cambia de estado
void ADispareitorModoJuego::OnMatchStateSet() {
    Super::OnMatchStateSet();

    for(FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It) {
        ADispareitorControladorJugador* DispareitorControladorJugador = Cast<ADispareitorControladorJugador>(*It);
        if(DispareitorControladorJugador) {
            DispareitorControladorJugador->ActualizarEstadoPartida(MatchState);
        }
    }
}

// Llamado por ADispareitorPersonaje::RecibirDanio
void ADispareitorModoJuego::JugadorEliminado(class ADispareitorPersonaje* DispareitorPersonajeVictima, class ADispareitorControladorJugador* DispareitorControladorJugadorVictima, class ADispareitorControladorJugador* DispareitorControladorJugadorAtacante) {
    ADispareitorEstadoJugador* AtacanteEstadoJugador = DispareitorControladorJugadorAtacante ? Cast<ADispareitorEstadoJugador>(DispareitorControladorJugadorAtacante->PlayerState) : nullptr;
    ADispareitorEstadoJugador* VictimaEstadoJugador = DispareitorControladorJugadorVictima ? Cast<ADispareitorEstadoJugador>(DispareitorControladorJugadorVictima->PlayerState) : nullptr;

    ADispareitorEstadoJuego* DispareitorEstadoJuego = GetGameState<ADispareitorEstadoJuego>();

    if(AtacanteEstadoJugador && AtacanteEstadoJugador != VictimaEstadoJugador && DispareitorEstadoJuego) {
        AtacanteEstadoJugador->IncrementarMuertos(1.f);
        DispareitorEstadoJuego->ActualizarArrayDeEstadoJugadoresConPuntuacionMasAlta(AtacanteEstadoJugador);
    }
    if(VictimaEstadoJugador) {
        VictimaEstadoJugador->IncrementarMuertes(1);
    }

    if(DispareitorPersonajeVictima) {
        DispareitorPersonajeVictima->Eliminado(false);
    }
}

// Llamado por ADispareitorPersonaje::TemporizadorEliminadoFinalizado
void ADispareitorModoJuego::PeticionReaparecer(ACharacter* PersonajeEliminado, AController* ControladorEliminado) {
    if(PersonajeEliminado) {
        PersonajeEliminado->Reset(); // Desune el controlador del caracter
        PersonajeEliminado->Destroy(); // Destruye al caracter pero el controlador sobrevive asi como otras clases como player state
    }    

    // Reaparecer lo mas alejado de los enemigos
    if(ControladorEliminado) {
        TArray<AActor*> IniciosJugador; 
        UGameplayStatics::GetAllActorsOfClass(this, APlayerStart::StaticClass(), IniciosJugador);

        AActor* InicioJugadorMasLejano = nullptr; 
        float DistanciaMasLejana = 0.f;
        for(AActor* InicioJugador : IniciosJugador) {
            float DistanciaEntreInicioJugadorYPersonajeMasCercana = MAX_flt; 
            for(FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It) {
                ADispareitorControladorJugador* DispareitorControladorJugador = Cast<ADispareitorControladorJugador>(*It);
                if(DispareitorControladorJugador != ControladorEliminado) {
                    ADispareitorPersonaje* DispareitorPersonaje = Cast<ADispareitorPersonaje>(DispareitorControladorJugador->GetPawn());
                    float DistanciaEntreInicioJugadorYPersonaje = InicioJugador->GetDistanceTo(DispareitorPersonaje);
                    if(DistanciaEntreInicioJugadorYPersonaje < DistanciaEntreInicioJugadorYPersonajeMasCercana) {
                        DistanciaEntreInicioJugadorYPersonajeMasCercana = DistanciaEntreInicioJugadorYPersonaje;
                    }
                }
            }

            if(DistanciaEntreInicioJugadorYPersonajeMasCercana > DistanciaMasLejana) {
                DistanciaMasLejana = DistanciaEntreInicioJugadorYPersonajeMasCercana;
                InicioJugadorMasLejano = InicioJugador;
            }
        }
        if(InicioJugadorMasLejano) {
            RestartPlayerAtPlayerStart(ControladorEliminado, InicioJugadorMasLejano);
        } else {
            int32 IndiceIniciosJugadorElegido = FMath::RandRange(0, IniciosJugador.Num() - 1);
            RestartPlayerAtPlayerStart(ControladorEliminado, IniciosJugador[IndiceIniciosJugadorElegido]);
        }
    }
}

//LLamado por BeginPlay
void ADispareitorModoJuego::SituarArmas() {
	TArray<AActor*> PuntosReaparicionArmas; 
    UGameplayStatics::GetAllActorsWithTag(this, "PuntoReaparicionArma", PuntosReaparicionArmas);

    TArray<AActor*> Armas; 
    UGameplayStatics::GetAllActorsOfClass(this, AArma::StaticClass(), Armas);

    int32 IndicePuntosReaparicionArmas = 0;
    for(AActor* Arma : Armas) {
        Arma->SetActorLocation(PuntosReaparicionArmas[IndicePuntosReaparicionArmas++]->GetActorLocation());
    }
}

void ADispareitorModoJuego::JugadorDejaJuego(ADispareitorEstadoJugador* DispareitorEstadoJugador) {
    if(!DispareitorEstadoJugador) {
        return;
    }
    
    ADispareitorEstadoJuego* DispareitorEstadoJuego = GetGameState<ADispareitorEstadoJuego>();
    if(DispareitorEstadoJuego && DispareitorEstadoJuego->ArrayDeEstadoJugadoresConPuntuacionMasAlta.Contains(DispareitorEstadoJugador)) {
        DispareitorEstadoJuego->ArrayDeEstadoJugadoresConPuntuacionMasAlta.Remove(DispareitorEstadoJugador);
    }

    ADispareitorPersonaje* DispareitorPersonaje = Cast<ADispareitorPersonaje>(DispareitorEstadoJugador->GetPawn());
    if(DispareitorPersonaje) {
        DispareitorPersonaje->Eliminado(true);
    }
}