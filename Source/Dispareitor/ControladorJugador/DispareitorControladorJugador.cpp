#include "DispareitorControladorJugador.h"
#include "Dispareitor/HUD/DispareitorHUD.h"
#include "Dispareitor/HUD/PantallaDelPersonaje.h"
#include "Dispareitor/HUD/AnunciosWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Dispareitor/Personaje/DispareitorPersonaje.h"
#include "Net/UnrealNetwork.h"
#include "Dispareitor/ModoJuego/DispareitorModoJuego.h"
#include "Kismet/GameplayStatics.h"
#include "Dispareitor/DispareitorComponentes/CombateComponente.h"
#include "Dispareitor/EstadoJuego/DispareitorEstadoJuego.h"
#include "Dispareitor/EstadoJugador/DispareitorEstadoJugador.h"


// APlayerController solo existe en el servidor y en el cliente propietario. Permite el acceso al HUD: vida, muertos, muertes, municion...

void ADispareitorControladorJugador::BeginPlay() {
    Super::BeginPlay();

    DispareitorHUD = Cast<ADispareitorHUD>(GetHUD());
    PartidaEstadoComprobar_EnServidor();
}

void ADispareitorControladorJugador::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(ADispareitorControladorJugador, PartidaEstado);
}


void ADispareitorControladorJugador::Tick(float DeltaTime) {
    Super::Tick(DeltaTime);
    HUDTiempoActivar();
    TiempoSincronizacionComprobar(DeltaTime);
    SondearInicio();
}

void ADispareitorControladorJugador::SondearInicio() {
    if(PantallaDelPersonaje == nullptr) {
        if(DispareitorHUD && DispareitorHUD->PantallaDelPersonaje) {
            PantallaDelPersonaje = DispareitorHUD->PantallaDelPersonaje;  
            if(PantallaDelPersonaje) {
                HUDVidaActualizar(HUDVida, HUDVidaMaxima);
                HUDMuertosActualizar(HUDMuertos);
                HUDMuertesActualizar(HUDMuertes);
            }
        }
    }
}

// ¿Llamado de forma indirecta por ADispareitorModoJuego::PeticionReaparecer?
void ADispareitorControladorJugador::OnPossess(APawn* Peon) {
    Super::OnPossess(Peon);

    ADispareitorPersonaje* DispareitorPersonaje = Cast<ADispareitorPersonaje>(Peon);
    if(DispareitorPersonaje) {
        HUDVidaActualizar(DispareitorPersonaje->ObtenerVida(), DispareitorPersonaje->ObtenerVidaMaxima());
    }
}

// Llamado por OnPossess y ADispareitorPersonaje::ActualizarHUDVida 
void ADispareitorControladorJugador::HUDVidaActualizar(float Vida, float VidaMaxima) {
    DispareitorHUD = DispareitorHUD != nullptr ? DispareitorHUD : Cast<ADispareitorHUD>(GetHUD());

    if(DispareitorHUD && DispareitorHUD->PantallaDelPersonaje && DispareitorHUD->PantallaDelPersonaje->VidaBarra && DispareitorHUD->PantallaDelPersonaje->VidaTexto) {
        DispareitorHUD->PantallaDelPersonaje->VidaBarra->SetPercent(Vida / VidaMaxima); 
        FString VidaTexto = FString::Printf(TEXT("%d/%d"), FMath::CeilToInt(Vida), FMath::CeilToInt(VidaMaxima));
        DispareitorHUD->PantallaDelPersonaje->VidaTexto->SetText(FText::FromString(VidaTexto)); 
    } else {
        bPantallaDelPersonajeInicializada = true;
        HUDVida = Vida;
        HUDVidaMaxima = VidaMaxima;

    }
}

// Llamado por ADispareitorEstadoJugador::IncrementarMuertos
void ADispareitorControladorJugador::HUDMuertosActualizar(float Muertos) {
    DispareitorHUD = DispareitorHUD != nullptr ? DispareitorHUD : Cast<ADispareitorHUD>(GetHUD());

    if(DispareitorHUD && DispareitorHUD->PantallaDelPersonaje && DispareitorHUD->PantallaDelPersonaje->Muertos) {
        FString MuertosTexto = FString::Printf(TEXT("%d"), FMath::FloorToInt(Muertos));
        DispareitorHUD->PantallaDelPersonaje->Muertos->SetText(FText::FromString(MuertosTexto));   
    } else {
        bPantallaDelPersonajeInicializada = true;
        HUDMuertos = Muertos;
    }
}

// Llamado por ADispareitorEstadoJugador::IncrementarMuertes
void ADispareitorControladorJugador::HUDMuertesActualizar(int32 Muertes) {
    DispareitorHUD = DispareitorHUD != nullptr ? DispareitorHUD : Cast<ADispareitorHUD>(GetHUD());

    if(DispareitorHUD && DispareitorHUD->PantallaDelPersonaje && DispareitorHUD->PantallaDelPersonaje->Muertes) {
        FString MuertesTexto = FString::Printf(TEXT("%d"), Muertes);
        DispareitorHUD->PantallaDelPersonaje->Muertes->SetText(FText::FromString(MuertesTexto));   
    } else {
        bPantallaDelPersonajeInicializada = true;
        HUDMuertes = Muertes;
    }
}

void ADispareitorControladorJugador::HUDArmaMunicionActualizar(int32 ArmaMunicion) {
    DispareitorHUD = DispareitorHUD != nullptr ? DispareitorHUD : Cast<ADispareitorHUD>(GetHUD());

    if(DispareitorHUD && DispareitorHUD->PantallaDelPersonaje && DispareitorHUD->PantallaDelPersonaje->MunicionArma) {
        FString ArmaMunicionTexto = FString::Printf(TEXT("%d"), ArmaMunicion);
        DispareitorHUD->PantallaDelPersonaje->MunicionArma->SetText(FText::FromString(ArmaMunicionTexto));   
    }
}

void ADispareitorControladorJugador::HUDPersonajeMunicionActualizar(int32 PersonajeMunicion) {
    DispareitorHUD = DispareitorHUD != nullptr ? DispareitorHUD : Cast<ADispareitorHUD>(GetHUD());

    if(DispareitorHUD && DispareitorHUD->PantallaDelPersonaje && DispareitorHUD->PantallaDelPersonaje->MunicionPersonaje) {
        FString PersonajeMunicionTexto = FString::Printf(TEXT("%d"), PersonajeMunicion);
        DispareitorHUD->PantallaDelPersonaje->MunicionPersonaje->SetText(FText::FromString(PersonajeMunicionTexto));   
    }
}

void ADispareitorControladorJugador::HUDPartidaTiempoActualizar(float CuentaAtrasTiempo) {
    DispareitorHUD = DispareitorHUD != nullptr ? DispareitorHUD : Cast<ADispareitorHUD>(GetHUD());

    if(DispareitorHUD && DispareitorHUD->PantallaDelPersonaje && DispareitorHUD->PantallaDelPersonaje->Tiempo) {
        if(CuentaAtrasTiempo >= 0.f) {  
            int32 Minutos = FMath::FloorToInt(CuentaAtrasTiempo / 60.f);
            int32 Segundos = CuentaAtrasTiempo - Minutos * 60;
            FString PartidaTiempoTexto = FString::Printf(TEXT("%02d:%02d"), Minutos, Segundos);
            DispareitorHUD->PantallaDelPersonaje->Tiempo->SetText(FText::FromString(PartidaTiempoTexto)); 
        } else {
            DispareitorHUD->PantallaDelPersonaje->Tiempo->SetText(FText()); 
        }
    }
}

void ADispareitorControladorJugador::HUDAnunciosTiempoActualizar(float CuentaAtrasTiempo) {
    DispareitorHUD = DispareitorHUD != nullptr ? DispareitorHUD : Cast<ADispareitorHUD>(GetHUD());

    if(DispareitorHUD && DispareitorHUD->AnunciosWidget && DispareitorHUD->AnunciosWidget->CalentamientoTiempo) {
        if(CuentaAtrasTiempo >= 0.f) {            
            int32 Minutos = FMath::FloorToInt(CuentaAtrasTiempo / 60.f);
            int32 Segundos = CuentaAtrasTiempo - Minutos * 60;
            FString CalentamientoTiempoTexto = FString::Printf(TEXT("%02d:%02d"), Minutos, Segundos);
            DispareitorHUD->AnunciosWidget->CalentamientoTiempo->SetText(FText::FromString(CalentamientoTiempoTexto));   
        } else {
            DispareitorHUD->AnunciosWidget->CalentamientoTiempo->SetText(FText());   
        }
    }
}

void ADispareitorControladorJugador::HUDTiempoActivar() {
    float TiempoRestante = 0.f;
    if(PartidaEstado == MatchState::WaitingToStart) {
        TiempoRestante = CalentamientoTiempo - ServidorTiempoObtener() + InicioNivelTiempo;
    } else if(PartidaEstado == MatchState::InProgress) {
        TiempoRestante = CalentamientoTiempo + PartidaTiempo - ServidorTiempoObtener() + InicioNivelTiempo;
    } else if(PartidaEstado == MatchState::Enfriamiento) {
        TiempoRestante = CalentamientoTiempo + PartidaTiempo + EnfriamientoTiempo - ServidorTiempoObtener() + InicioNivelTiempo;
    }
    uint32 SegundosRestantesTemporal = FMath::CeilToInt(TiempoRestante);

    if(HasAuthority()) {
        DispareitorModoJuego = DispareitorModoJuego != nullptr ? DispareitorModoJuego : Cast<ADispareitorModoJuego>(UGameplayStatics::GetGameMode(this));
        if(DispareitorModoJuego) {
            SegundosRestantesTemporal = FMath::CeilToInt(DispareitorModoJuego->CuentaAtrasTiempoObtener() + InicioNivelTiempo);
        }   
    }

    if(SegundosRestantesTemporal != SegundosRestantes) {
        if(PartidaEstado == MatchState::WaitingToStart || PartidaEstado == MatchState::Enfriamiento) {
            HUDAnunciosTiempoActualizar(TiempoRestante);
        } else if(PartidaEstado == MatchState::InProgress) {
            HUDPartidaTiempoActualizar(TiempoRestante);
        }   
    }
    SegundosRestantes = SegundosRestantesTemporal;
}

// Peticion tiempo (ejecutada en servidor) cliente -> servidor 
void ADispareitorControladorJugador::TiempoServidorPeticion_EnServidor_Implementation(float TiempoClientePeticion) {
    float TiempoServidorAlRecibirPeticion = GetWorld()->GetTimeSeconds();
    TiempoServidorDevolucion_EnCliente(TiempoClientePeticion, TiempoServidorAlRecibirPeticion);
}

// Respuesta tiempo (ejecutada en cliente) servidor -> cliente
void ADispareitorControladorJugador::TiempoServidorDevolucion_EnCliente_Implementation(float TiempoClientePeticion, float TiempoServidorAlRecibirPeticion) {
    // RoundTripTime
    float RTT = GetWorld()->GetTimeSeconds() - TiempoClientePeticion;
    float TiempoServidorActual = TiempoServidorAlRecibirPeticion +(0.5f * RTT);
    TiempoServidorClienteDelta = TiempoServidorActual - GetWorld()->GetTimeSeconds();
}

float ADispareitorControladorJugador::ServidorTiempoObtener() {
    return HasAuthority() ? GetWorld()->GetTimeSeconds() : GetWorld()->GetTimeSeconds() + TiempoServidorClienteDelta;
}

// El momento mas temprano en el que podemos sincronizar con el servidor
void ADispareitorControladorJugador::ReceivedPlayer() {
    Super::ReceivedPlayer();
    if(IsLocalController()) {
        TiempoServidorPeticion_EnServidor(GetWorld()->GetTimeSeconds());
    }
}

void ADispareitorControladorJugador::TiempoSincronizacionComprobar(float DeltaTime) {
    TiempoSincronizacionPasado += DeltaTime;
    if(IsLocalController() && TiempoSincronizacionPasado > TiempoSincronizacionFrecuencia) {
        TiempoServidorPeticion_EnServidor(GetWorld()->GetTimeSeconds());
        TiempoSincronizacionPasado = 0.f;
    }
}

// Llamado por ADispareitorModoJuego::OnMatchStateSet
void ADispareitorControladorJugador::PartidaEstadoActualizar(FName Estado) {
    PartidaEstado = Estado;
    PartidaEstadoManejador();
}

void ADispareitorControladorJugador::PartidaEstado_AlReplicar() {
    PartidaEstadoManejador();
}

void ADispareitorControladorJugador::PartidaEstadoManejador() {
    DispareitorHUD = DispareitorHUD != nullptr ? DispareitorHUD : Cast<ADispareitorHUD>(GetHUD());
    if(DispareitorHUD) {
        if(PartidaEstado == MatchState::InProgress) {
            DispareitorHUD->AnadirPantallaDelPersonaje();
            if(DispareitorHUD->AnunciosWidget) {
                DispareitorHUD->AnunciosWidget->SetVisibility(ESlateVisibility::Hidden);
            }
        } else if(PartidaEstado == MatchState::Enfriamiento) {
            DispareitorHUD->PantallaDelPersonaje->RemoveFromParent();
            if(DispareitorHUD->AnunciosWidget && DispareitorHUD->AnunciosWidget->PartidaComienza && DispareitorHUD->AnunciosWidget->Informacion) {
                DispareitorHUD->AnunciosWidget->SetVisibility(ESlateVisibility::Visible);
                
                FString PartidaComienzaTexto("Nueva partida comienza en:");
                DispareitorHUD->AnunciosWidget->PartidaComienza->SetText(FText::FromString(PartidaComienzaTexto));
                
                ADispareitorEstadoJuego* DispareitorEstadoJuego = Cast<ADispareitorEstadoJuego>(UGameplayStatics::GetGameState(this));
                ADispareitorEstadoJugador* DispareitorEstadoJugador = GetPlayerState<ADispareitorEstadoJugador>();
                if(DispareitorEstadoJuego && DispareitorEstadoJugador) {
                    TArray<ADispareitorEstadoJugador*> EstadoJugadoresPuntuacionMasAlta = DispareitorEstadoJuego->EstadoJugadoresPuntuacionMasAlta;
                    FString JugadoresPuntuacionMasAltaTexto;
                    if(EstadoJugadoresPuntuacionMasAlta.Num() == 0) {
                        JugadoresPuntuacionMasAltaTexto = FString("No hay ganador. Sois todos unos losers!"); 
                    } else if(EstadoJugadoresPuntuacionMasAlta.Num() == 1) {
                        if(EstadoJugadoresPuntuacionMasAlta[0] == DispareitorEstadoJugador) {
                            JugadoresPuntuacionMasAltaTexto = FString("Eres el ganador. El puto amo de Dispareitor!");
                        } else {
                            JugadoresPuntuacionMasAltaTexto = FString::Printf(TEXT("Apunta bien su nombre para la siguiente...\n Os ha barrido de la partida: %s"), *EstadoJugadoresPuntuacionMasAlta[0]->GetPlayerName());
                        }
                    } else if(EstadoJugadoresPuntuacionMasAlta.Num() > 1) {
                        JugadoresPuntuacionMasAltaTexto = FString("Han quedado empatados en el 'namber guan':\n");
                        for(auto EstadoJugador : EstadoJugadoresPuntuacionMasAlta) {
                            JugadoresPuntuacionMasAltaTexto.Append(FString::Printf(TEXT("%s\n"), *EstadoJugador->GetPlayerName()));    
                        }
                    }

                    DispareitorHUD->AnunciosWidget->Informacion->SetText(FText::FromString(JugadoresPuntuacionMasAltaTexto));
                }
            }
        }
    } 
    
    if(PartidaEstado == MatchState::Enfriamiento) {
        ADispareitorPersonaje* DispareitorPersonaje = Cast<ADispareitorPersonaje>(GetPawn());
        if(DispareitorPersonaje && DispareitorPersonaje->CombateComponenteObtener()) {
            DispareitorPersonaje->bSoloGirarCamara = true;
            DispareitorPersonaje->CombateComponenteObtener()->DispararPresionado(false);
        }
    }
}

void ADispareitorControladorJugador::PartidaEstadoComprobar_EnServidor_Implementation() {
    DispareitorModoJuego = DispareitorModoJuego != nullptr ? DispareitorModoJuego : Cast<ADispareitorModoJuego>(UGameplayStatics::GetGameMode(this));
    if(DispareitorModoJuego) {
        CalentamientoTiempo = DispareitorModoJuego->CalentamientoTiempo;
        PartidaTiempo = DispareitorModoJuego->PartidaTiempo;
        EnfriamientoTiempo = DispareitorModoJuego->EnfriamientoTiempo;
        InicioNivelTiempo = DispareitorModoJuego->InicioNivelTiempo;
        PartidaEstado = DispareitorModoJuego->GetMatchState();
        PartidaEstadoComprobar_EnCliente(PartidaEstado, CalentamientoTiempo, PartidaTiempo, EnfriamientoTiempo, InicioNivelTiempo);
    }
}

void ADispareitorControladorJugador::PartidaEstadoComprobar_EnCliente_Implementation(FName _PartidaEstado, float _CalentamientoTiempo, float _PartidaTiempo, float _EnfriamientoTiempo, float _InicioNivelTiempo) {
    CalentamientoTiempo = _CalentamientoTiempo;
    PartidaTiempo = _PartidaTiempo;
    EnfriamientoTiempo = _EnfriamientoTiempo;
    InicioNivelTiempo = _InicioNivelTiempo;
    PartidaEstado = _PartidaEstado;
    PartidaEstadoActualizar(PartidaEstado);

    // Si nos unimos cuando la partida ya esté empezada no veremos este anuncio
    if(DispareitorHUD && PartidaEstado == MatchState::WaitingToStart) {
        DispareitorHUD->AnadirAnunciosWidget();
    }
}

