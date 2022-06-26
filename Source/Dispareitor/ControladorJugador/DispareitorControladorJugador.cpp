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
    ActivarHUDTiempo();
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

void ADispareitorControladorJugador::HUDPartidaTiempoActualizar(float _PartidaTiempo) {
    DispareitorHUD = DispareitorHUD != nullptr ? DispareitorHUD : Cast<ADispareitorHUD>(GetHUD());

    if(DispareitorHUD && DispareitorHUD->PantallaDelPersonaje && DispareitorHUD->PantallaDelPersonaje->Tiempo) {
        int32 Minutos = FMath::FloorToInt(_PartidaTiempo / 60.f);
        int32 Segundos = _PartidaTiempo - Minutos * 60;
        FString PartidaTiempoTexto = FString::Printf(TEXT("%02d:%02d"), Minutos, Segundos);
        DispareitorHUD->PantallaDelPersonaje->Tiempo->SetText(FText::FromString(PartidaTiempoTexto));   
    }
}

void ADispareitorControladorJugador::HUDCalentamientoTiempoActualizar(float _CalentamientoTiempo) {
    DispareitorHUD = DispareitorHUD != nullptr ? DispareitorHUD : Cast<ADispareitorHUD>(GetHUD());

    if(DispareitorHUD && DispareitorHUD->AnunciosWidget && DispareitorHUD->AnunciosWidget->CalentamientoTiempo) {
        int32 Minutos = FMath::FloorToInt(_CalentamientoTiempo / 60.f);
        int32 Segundos = _CalentamientoTiempo - Minutos * 60;
        FString CalentamientoTiempoTexto = FString::Printf(TEXT("%02d:%02d"), Minutos, Segundos);
        DispareitorHUD->AnunciosWidget->CalentamientoTiempo->SetText(FText::FromString(CalentamientoTiempoTexto));   
    }
}

void ADispareitorControladorJugador::ActivarHUDTiempo() {
    float TiempoRestante = 0.f;
    if(PartidaEstado == MatchState::WaitingToStart) {
        TiempoRestante = CalentamientoTiempo - ServidorTiempoObtener() + InicioNivelTiempo;
    } else if(PartidaEstado == MatchState::InProgress) {
        TiempoRestante = CalentamientoTiempo + PartidaTiempo - ServidorTiempoObtener() + InicioNivelTiempo;
    }

    uint32 SegundosRestantesTemporal = FMath::CeilToInt(TiempoRestante);
    if(SegundosRestantesTemporal != SegundosRestantes) {
        if(PartidaEstado == MatchState::WaitingToStart) {
            HUDCalentamientoTiempoActualizar(TiempoRestante);
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
            if(DispareitorHUD->AnunciosWidget) {
                DispareitorHUD->AnunciosWidget->SetVisibility(ESlateVisibility::Visible);
            }
        }
    }
}

void ADispareitorControladorJugador::PartidaEstadoComprobar_EnServidor_Implementation() {
    ADispareitorModoJuego* DispareitorModoJuego = Cast<ADispareitorModoJuego>(UGameplayStatics::GetGameMode(this));
    if(DispareitorModoJuego) {
        CalentamientoTiempo = DispareitorModoJuego->CalentamientoTiempo;
        PartidaTiempo = DispareitorModoJuego->PartidaTiempo;
        InicioNivelTiempo = DispareitorModoJuego->InicioNivelTiempo;
        PartidaEstado = DispareitorModoJuego->GetMatchState();
        PartidaEstadoComprobar_EnCliente(PartidaEstado, CalentamientoTiempo, PartidaTiempo, InicioNivelTiempo);
        
        if(DispareitorHUD && PartidaEstado == MatchState::WaitingToStart) {
            DispareitorHUD->AnadirAnunciosWidget();
        }
    }
}

void ADispareitorControladorJugador::PartidaEstadoComprobar_EnCliente_Implementation(FName _PartidaEstado, float _CalentamientoTiempo, float _PartidaTiempo, float _InicioNivelTiempo) {
    CalentamientoTiempo = _CalentamientoTiempo;
    PartidaTiempo = _PartidaTiempo;
    InicioNivelTiempo = _InicioNivelTiempo;
    PartidaEstado = _PartidaEstado;
    PartidaEstadoActualizar(PartidaEstado);

    // Si nos unimos cuando la partida ya esté empezada no veremos este anuncio
    if(DispareitorHUD && PartidaEstado == MatchState::WaitingToStart) {
        DispareitorHUD->AnadirAnunciosWidget();
    }
}

