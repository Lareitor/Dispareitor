#include "DispareitorModoJuego.h"
#include "Dispareitor/Personaje/DispareitorPersonaje.h"
#include "Dispareitor/ControladorJugador/DispareitorControladorJugador.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"
#include "Dispareitor/EstadoJugador/DispareitorEstadoJugador.h"
#include "Dispareitor/EstadoJuego/DispareitorEstadoJuego.h"
#include "Math/NumericLimits.h"


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

    InicioNivelTiempo = GetWorld()->GetTimeSeconds();
}

void ADispareitorModoJuego::Tick(float DeltaTime) {
    Super::Tick(DeltaTime);

    if(MatchState == MatchState::WaitingToStart) {
        CuentaAtrasTiempo = CalentamientoTiempo - GetWorld()->GetTimeSeconds() + InicioNivelTiempo;
        if(CuentaAtrasTiempo <= 0.f) {
            // Pasamos del estado WaitingToStart a InProgress donde los jugadores aparecen ya con las mallas
            StartMatch();
        }
    } else if(MatchState == MatchState::InProgress) {
        CuentaAtrasTiempo = CalentamientoTiempo + PartidaTiempo - GetWorld()->GetTimeSeconds() + InicioNivelTiempo;
        if(CuentaAtrasTiempo <= 0.f) {
            SetMatchState(MatchState::Enfriamiento);
        }
    } else if(MatchState == MatchState::Enfriamiento) {
        CuentaAtrasTiempo = CalentamientoTiempo + PartidaTiempo + EnfriamientoTiempo - GetWorld()->GetTimeSeconds() + InicioNivelTiempo;
        if(CuentaAtrasTiempo <= 0.f) {
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
            DispareitorControladorJugador->PartidaEstadoActualizar(MatchState);
        }
    }
}

// Llamado por ADispareitorPersonaje::RecibirDano
void ADispareitorModoJuego::JugadorEliminado(class ADispareitorPersonaje* VictimaDispareitorJugador, class ADispareitorControladorJugador* VictimaDispareitorControladorJugador, class ADispareitorControladorJugador* AtacanteDispareitorControladorJugador) {
    ADispareitorEstadoJugador* AtacanteEstadoJugador = AtacanteDispareitorControladorJugador ? Cast<ADispareitorEstadoJugador>(AtacanteDispareitorControladorJugador->PlayerState) : nullptr;
    ADispareitorEstadoJugador* VictimaEstadoJugador = VictimaDispareitorControladorJugador ? Cast<ADispareitorEstadoJugador>(VictimaDispareitorControladorJugador->PlayerState) : nullptr;

    ADispareitorEstadoJuego* DispareitorEstadoJuego = GetGameState<ADispareitorEstadoJuego>();

    if(AtacanteEstadoJugador && AtacanteEstadoJugador != VictimaEstadoJugador && DispareitorEstadoJuego) {
        AtacanteEstadoJugador->IncrementarMuertos(1.f);
        DispareitorEstadoJuego->EstadoJugadoresPuntuacionMasAltaActualizar(AtacanteEstadoJugador);
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

    // Reaparecer lo mas alejado de los enemigos
    if(ControladorEliminado) {
        TArray<AActor*> IniciosDeJugador; 
        UGameplayStatics::GetAllActorsOfClass(this, APlayerStart::StaticClass(), IniciosDeJugador);

        AActor* InicioDeJugadorMasLejano = nullptr; 
        float DistanciaMasLejana = 0.f;
        for(AActor* InicioDeJugador : IniciosDeJugador) {
            float InicioDeJugadorDistanciaMasCercanaAJugador = MAX_flt; 
            for(FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It) {
                ADispareitorControladorJugador* DispareitorControladorJugador = Cast<ADispareitorControladorJugador>(*It);
                if(DispareitorControladorJugador != ControladorEliminado) {
                    ADispareitorPersonaje* DispareitorPersonaje = Cast<ADispareitorPersonaje>(DispareitorControladorJugador->GetPawn());
                    float DistanciaEntreInicioDeJugadorYPersonaje = InicioDeJugador->GetDistanceTo(DispareitorPersonaje);
                    if(DistanciaEntreInicioDeJugadorYPersonaje < InicioDeJugadorDistanciaMasCercanaAJugador) {
                        InicioDeJugadorDistanciaMasCercanaAJugador = DistanciaEntreInicioDeJugadorYPersonaje;
                    }
                }
            }

            if(InicioDeJugadorDistanciaMasCercanaAJugador > DistanciaMasLejana) {
                DistanciaMasLejana = InicioDeJugadorDistanciaMasCercanaAJugador;
                InicioDeJugadorMasLejano = InicioDeJugador;
            }
        }
        if(InicioDeJugadorMasLejano) {
            RestartPlayerAtPlayerStart(ControladorEliminado, InicioDeJugadorMasLejano);
        } else {
            int32 IndiceIniciosDeJugadorElegido = FMath::RandRange(0, IniciosDeJugador.Num() - 1);
            RestartPlayerAtPlayerStart(ControladorEliminado, IniciosDeJugador[IndiceIniciosDeJugadorElegido]);
        }
    }
}
