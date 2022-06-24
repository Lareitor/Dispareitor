#include "DispareitorControladorJugador.h"
#include "Dispareitor/HUD/DispareitorHUD.h"
#include "Dispareitor/HUD/PantallaDelPersonaje.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Dispareitor/Personaje/DispareitorPersonaje.h"
#include "Net/UnrealNetwork.h"
#include "Dispareitor/ModoJuego/DispareitorModoJuego.h"

// APlayerController solo existe en el servidor y en el cliente propietario. Permite el acceso al HUD: vida, muertos, muertes, municion...

void ADispareitorControladorJugador::BeginPlay() {
    Super::BeginPlay();

    DispareitorHUD = Cast<ADispareitorHUD>(GetHUD());
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
                ActualizarHUDVida(HUDVida, HUDVidaMaxima);
                ActualizarHUDMuertos(HUDMuertos);
                ActualizarHUDMuertes(HUDMuertes);
            }
        }
    }
}

// ¿Llamado de forma indirecta por ADispareitorModoJuego::PeticionReaparecer?
void ADispareitorControladorJugador::OnPossess(APawn* Peon) {
    Super::OnPossess(Peon);

    ADispareitorPersonaje* DispareitorPersonaje = Cast<ADispareitorPersonaje>(Peon);
    if(DispareitorPersonaje) {
        ActualizarHUDVida(DispareitorPersonaje->ObtenerVida(), DispareitorPersonaje->ObtenerVidaMaxima());
    }
}

// Llamado por OnPossess y ADispareitorPersonaje::ActualizarHUDVida 
void ADispareitorControladorJugador::ActualizarHUDVida(float Vida, float VidaMaxima) {
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
void ADispareitorControladorJugador::ActualizarHUDMuertos(float Muertos) {
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
void ADispareitorControladorJugador::ActualizarHUDMuertes(int32 Muertes) {
    DispareitorHUD = DispareitorHUD != nullptr ? DispareitorHUD : Cast<ADispareitorHUD>(GetHUD());

    if(DispareitorHUD && DispareitorHUD->PantallaDelPersonaje && DispareitorHUD->PantallaDelPersonaje->Muertes) {
        FString MuertesTexto = FString::Printf(TEXT("%d"), Muertes);
        DispareitorHUD->PantallaDelPersonaje->Muertes->SetText(FText::FromString(MuertesTexto));   
    } else {
        bPantallaDelPersonajeInicializada = true;
        HUDMuertes = Muertes;
    }
}

void ADispareitorControladorJugador::ActualizarHUDMunicionArma(int32 MunicionArma) {
    DispareitorHUD = DispareitorHUD != nullptr ? DispareitorHUD : Cast<ADispareitorHUD>(GetHUD());

    if(DispareitorHUD && DispareitorHUD->PantallaDelPersonaje && DispareitorHUD->PantallaDelPersonaje->MunicionArma) {
        FString MunicionArmaTexto = FString::Printf(TEXT("%d"), MunicionArma);
        DispareitorHUD->PantallaDelPersonaje->MunicionArma->SetText(FText::FromString(MunicionArmaTexto));   
    }
}

void ADispareitorControladorJugador::ActualizarHUDMunicionPersonaje(int32 MunicionPersonaje) {
    DispareitorHUD = DispareitorHUD != nullptr ? DispareitorHUD : Cast<ADispareitorHUD>(GetHUD());

    if(DispareitorHUD && DispareitorHUD->PantallaDelPersonaje && DispareitorHUD->PantallaDelPersonaje->MunicionPersonaje) {
        FString MunicionPersonajeTexto = FString::Printf(TEXT("%d"), MunicionPersonaje);
        DispareitorHUD->PantallaDelPersonaje->MunicionPersonaje->SetText(FText::FromString(MunicionPersonajeTexto));   
    }
}

void ADispareitorControladorJugador::ActualizarHUDTiempo(float Tiempo) {
    DispareitorHUD = DispareitorHUD != nullptr ? DispareitorHUD : Cast<ADispareitorHUD>(GetHUD());

    if(DispareitorHUD && DispareitorHUD->PantallaDelPersonaje && DispareitorHUD->PantallaDelPersonaje->Tiempo) {
        int32 Minutos = FMath::FloorToInt(Tiempo / 60.f);
        int32 Segundos = Tiempo - Minutos * 60;
        FString TiempoTexto = FString::Printf(TEXT("%02d:%02d"), Minutos, Segundos);
        DispareitorHUD->PantallaDelPersonaje->Tiempo->SetText(FText::FromString(TiempoTexto));   
    }
}

void ADispareitorControladorJugador::ActivarHUDTiempo() {
    uint32 SegundosRestantesTemporal = FMath::CeilToInt(PartidaTiempo - TiempoServidorObtener());
    if(SegundosRestantesTemporal != SegundosRestantes) {
        ActualizarHUDTiempo(PartidaTiempo - TiempoServidorObtener());
    }
    SegundosRestantes = SegundosRestantesTemporal;
}

// Peticion tiempo (ejecutada en servidor) cliente -> servidor 
void ADispareitorControladorJugador::TiempoServidorPeticion_Implementation(float TiempoClientePeticion) {
    float TiempoServidorAlRecibirPeticion = GetWorld()->GetTimeSeconds();
    TiempoServidorDevolucion(TiempoClientePeticion, TiempoServidorAlRecibirPeticion);
}

// Respuesta tiempo (ejecutada en cliente) servidor -> cliente
void ADispareitorControladorJugador::TiempoServidorDevolucion_Implementation(float TiempoClientePeticion, float TiempoServidorAlRecibirPeticion) {
    // RoundTripTime
    float RTT = GetWorld()->GetTimeSeconds() - TiempoClientePeticion;
    float TiempoServidorActual = TiempoServidorAlRecibirPeticion +(0.5f * RTT);
    TiempoServidorClienteDelta = TiempoServidorActual - GetWorld()->GetTimeSeconds();
}

float ADispareitorControladorJugador::TiempoServidorObtener() {
    return HasAuthority() ? GetWorld()->GetTimeSeconds() : GetWorld()->GetTimeSeconds() + TiempoServidorClienteDelta;
}

// El momento mas temprano en el que podemos sincronizar con el servidor
void ADispareitorControladorJugador::ReceivedPlayer() {
    Super::ReceivedPlayer();
    if(IsLocalController()) {
        TiempoServidorPeticion(GetWorld()->GetTimeSeconds());
    }
}

void ADispareitorControladorJugador::TiempoSincronizacionComprobar(float DeltaTime) {
    TiempoSincronizacionPasado += DeltaTime;
    if(IsLocalController() && TiempoSincronizacionPasado > TiempoSincronizacionFrecuencia) {
        TiempoServidorPeticion(GetWorld()->GetTimeSeconds());
        TiempoSincronizacionPasado = 0.f;
    }
}

void ADispareitorControladorJugador::PartidaEstadoActualizar(FName Estado) {
    PartidaEstado = Estado;

    if(PartidaEstado == MatchState::InProgress) {
        DispareitorHUD = DispareitorHUD != nullptr ? DispareitorHUD : Cast<ADispareitorHUD>(GetHUD());
        if(DispareitorHUD) {
            DispareitorHUD->AnadirPantallaDelPersonaje();
        }
    }
}

void ADispareitorControladorJugador::PartidaEstadoAlReplicar() {
     if(PartidaEstado == MatchState::InProgress) {
        DispareitorHUD = DispareitorHUD != nullptr ? DispareitorHUD : Cast<ADispareitorHUD>(GetHUD());
        if(DispareitorHUD) {
            DispareitorHUD->AnadirPantallaDelPersonaje();
        }
    }
}
