#include "DispareitorControladorJugador.h"
#include "Dispareitor/HUD/DispareitorHUD.h"
#include "Dispareitor/HUD/PantallaDelPersonaje.h"
#include "Dispareitor/HUD/AnunciosWidget.h"
#include "Dispareitor/HUD/FrancotiradorCruceta.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Dispareitor/Personaje/DispareitorPersonaje.h"
#include "Net/UnrealNetwork.h"
#include "Dispareitor/ModoJuego/DispareitorModoJuego.h"
#include "Kismet/GameplayStatics.h"
#include "Dispareitor/DispareitorComponentes/CombateComponente.h"
#include "Dispareitor/EstadoJuego/DispareitorEstadoJuego.h"
#include "Dispareitor/EstadoJugador/DispareitorEstadoJugador.h"
#include "Components/Image.h"
#include "Dispareitor/HUD/RegresarAMenuPrincipal.h"
#include "Dispareitor/Tipos/Anuncio.h"

// APlayerController solo existe en el servidor y en el cliente propietario. Permite el acceso al HUD: vida, muertos, muertes, municion...

void ADispareitorControladorJugador::BeginPlay() {
    Super::BeginPlay();

    DispareitorHUD = Cast<ADispareitorHUD>(GetHUD());
    ComprobarEstadoPartida_EnServidor();
}

// Esta funcion es parecida a ADispareitorPersonaje::SetupPlayerInputComponent pero en APlayerController
void ADispareitorControladorJugador::SetupInputComponent() {
    Super::SetupInputComponent();
    if(!InputComponent) {
        return;
    }

    // Enlazamos la tecla quitar en el controlador y no en el personaje porque si esta muerto el jugador no podemos mostrar el menu
    InputComponent->BindAction("Quitar", IE_Pressed, this, &ADispareitorControladorJugador::MostrarRegresarAMenuPrincipal); 
}

void ADispareitorControladorJugador::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(ADispareitorControladorJugador, EstadoPartida);
    DOREPLIFETIME(ADispareitorControladorJugador, bMostrarPuntuacionEquipos);
}


void ADispareitorControladorJugador::Tick(float DeltaTime) {
    Super::Tick(DeltaTime);
    ActivarTiempoHUD();
    ComprobarTiempoSincronizacion(DeltaTime);
    SondearInicio();
    ComprobarPingAlto(DeltaTime);
}

void ADispareitorControladorJugador::SondearInicio() {
    if(!PantallaDelPersonaje) {
        if(DispareitorHUD && DispareitorHUD->PantallaDelPersonaje) {
            PantallaDelPersonaje = DispareitorHUD->PantallaDelPersonaje;  
            if(PantallaDelPersonaje) {
                ADispareitorPersonaje* DispareitorPersonaje = Cast<ADispareitorPersonaje>(GetPawn());
                if(bInicializadaVida) {
                    ActualizarVidaHUD(VidaHUD, VidaMaximaHUD);
                }
                if(bInicializadoEscudo) {
                    ActualizarEscudoHUD(EscudoHUD, EscudoMaximoHUD);
                }
                if(bInicializadoMuertos) {
                    ActualizarMuertosHUD(MuertosHUD);
                }
                if(bInicializadoMuertes) {
                    ActualizarMuertesHUD(MuertesHUD);
                }
                if(bInicializadaMunicionPersonaje) {
                    ActualizarMunicionPersonajeHUD(MunicionPersonajeHUD);
                }
                if(bInicializadaMunicionArma) {
                    ActualizarMunicionArmaHUD(MunicionArmaHUD);
                }
                if(DispareitorPersonaje && DispareitorPersonaje->ObtenerCombateComponente()) {
                    if(bInicializadaGranadas) {
                        ActualizarGranadasHUD(GranadasActualesHUD);
                    }
                }
            }
        }
    }
}

// Llamado por BeginPlay
void ADispareitorControladorJugador::ComprobarEstadoPartida_EnServidor_Implementation() {
    DispareitorModoJuego = DispareitorModoJuego ? DispareitorModoJuego : Cast<ADispareitorModoJuego>(UGameplayStatics::GetGameMode(this));
    if(DispareitorModoJuego) {
        TiempoCalentamiento = DispareitorModoJuego->TiempoCalentamiento;
        TiempoPartida = DispareitorModoJuego->TiempoPartida;
        TiempoEnfriamiento = DispareitorModoJuego->TiempoEnfriamiento;
        TiempoInicioNivel = DispareitorModoJuego->TiempoInicioNivel;
        EstadoPartida = DispareitorModoJuego->GetMatchState();
        ComprobarEstadoPartida_EnCliente(EstadoPartida, TiempoCalentamiento, TiempoPartida, TiempoEnfriamiento, TiempoInicioNivel);
    }
}

void ADispareitorControladorJugador::ComprobarEstadoPartida_EnCliente_Implementation(FName _EstadoPartida, float _TiempoCalentamiento, float _TiempoPartida, float _TiempoEnfriamiento, float _TiempoInicioNivel) {
    TiempoCalentamiento = _TiempoCalentamiento;
    TiempoPartida = _TiempoPartida;
    TiempoEnfriamiento = _TiempoEnfriamiento;
    TiempoInicioNivel = _TiempoInicioNivel;
    EstadoPartida = _EstadoPartida;
    ActualizarEstadoPartida(EstadoPartida);

    // Si nos unimos cuando la partida ya esté empezada no veremos este anuncio
    if(DispareitorHUD && EstadoPartida == MatchState::WaitingToStart) {
        DispareitorHUD->MostrarAnunciosWidget();
    }
}

// Llamado de forma indirecta por ADispareitorModoJuego::PeticionReaparecer y al inicio de la partida, ¿se ejecuta solo en el server?
void ADispareitorControladorJugador::OnPossess(APawn* Peon) {
    Super::OnPossess(Peon);
}

// Llamado por OnPossess y ADispareitorPersonaje::ActualizarVidaHUD 
void ADispareitorControladorJugador::ActualizarVidaHUD(float Vida, float VidaMaxima) {
    DispareitorHUD = DispareitorHUD ? DispareitorHUD : Cast<ADispareitorHUD>(GetHUD());

    if(DispareitorHUD && DispareitorHUD->PantallaDelPersonaje && DispareitorHUD->PantallaDelPersonaje->BarraVida && DispareitorHUD->PantallaDelPersonaje->TextoVida) {
        DispareitorHUD->PantallaDelPersonaje->BarraVida->SetPercent(Vida / VidaMaxima); 
        FString TextoVida = FString::Printf(TEXT("%d/%d"), FMath::CeilToInt(Vida), FMath::CeilToInt(VidaMaxima));
        DispareitorHUD->PantallaDelPersonaje->TextoVida->SetText(FText::FromString(TextoVida)); 
    } else {
        bInicializadaVida = true;
        VidaHUD = Vida;
        VidaMaximaHUD = VidaMaxima;
    }
}

void ADispareitorControladorJugador::ActualizarEscudoHUD(float Escudo, float EscudoMaximo) {
    DispareitorHUD = DispareitorHUD ? DispareitorHUD : Cast<ADispareitorHUD>(GetHUD());

    if(DispareitorHUD && DispareitorHUD->PantallaDelPersonaje && DispareitorHUD->PantallaDelPersonaje->BarraEscudo && DispareitorHUD->PantallaDelPersonaje->TextoEscudo) {
        DispareitorHUD->PantallaDelPersonaje->BarraEscudo->SetPercent(Escudo / EscudoMaximo); 
        FString TextoEscudo = FString::Printf(TEXT("%d/%d"), FMath::CeilToInt(Escudo), FMath::CeilToInt(EscudoMaximo));
        DispareitorHUD->PantallaDelPersonaje->TextoEscudo->SetText(FText::FromString(TextoEscudo)); 
    } else {
        bInicializadoEscudo = true;
        EscudoHUD = Escudo;
        EscudoMaximoHUD = EscudoMaximo;

    }
}

// Llamado por ADispareitorEstadoJugador::IncrementarMuertos
void ADispareitorControladorJugador::ActualizarMuertosHUD(float Muertos) {
    DispareitorHUD = DispareitorHUD ? DispareitorHUD : Cast<ADispareitorHUD>(GetHUD());

    if(DispareitorHUD && DispareitorHUD->PantallaDelPersonaje && DispareitorHUD->PantallaDelPersonaje->Muertos) {
        FString MuertosTexto = FString::Printf(TEXT("%d"), FMath::FloorToInt(Muertos));
        DispareitorHUD->PantallaDelPersonaje->Muertos->SetText(FText::FromString(MuertosTexto));   
    } else {
        bInicializadoMuertos = true;
        MuertosHUD = Muertos;
    }
}

// Llamado por ADispareitorEstadoJugador::IncrementarMuertes
void ADispareitorControladorJugador::ActualizarMuertesHUD(int32 Muertes) {
    DispareitorHUD = DispareitorHUD ? DispareitorHUD : Cast<ADispareitorHUD>(GetHUD());

    if(DispareitorHUD && DispareitorHUD->PantallaDelPersonaje && DispareitorHUD->PantallaDelPersonaje->Muertes) {
        FString MuertesTexto = FString::Printf(TEXT("%d"), Muertes);
        DispareitorHUD->PantallaDelPersonaje->Muertes->SetText(FText::FromString(MuertesTexto));   
    } else {
        bInicializadoMuertes = true;
        MuertesHUD = Muertes;
    }
}

// Llamado por SondearInicio y UCombateComponente::ActualizarGranadasHUD
void ADispareitorControladorJugador::ActualizarGranadasHUD(int32 CantidadGranadas) {
    DispareitorHUD = DispareitorHUD ? DispareitorHUD : Cast<ADispareitorHUD>(GetHUD());

    if(DispareitorHUD && DispareitorHUD->PantallaDelPersonaje && DispareitorHUD->PantallaDelPersonaje->CantidadGranadas) {
        FString TextoCantidadGranadas = FString::Printf(TEXT("%d"), CantidadGranadas);
        DispareitorHUD->PantallaDelPersonaje->CantidadGranadas->SetText(FText::FromString(TextoCantidadGranadas));   
    } else {
        bInicializadaGranadas = true;
        GranadasActualesHUD = CantidadGranadas;
    }
}

void ADispareitorControladorJugador::ActualizarMunicionArmaHUD(int32 MunicionArma) {
    DispareitorHUD = DispareitorHUD ? DispareitorHUD : Cast<ADispareitorHUD>(GetHUD());

    if(DispareitorHUD && DispareitorHUD->PantallaDelPersonaje && DispareitorHUD->PantallaDelPersonaje->MunicionArma) {
        FString TextoMunicionArma = FString::Printf(TEXT("%d"), MunicionArma);
        DispareitorHUD->PantallaDelPersonaje->MunicionArma->SetText(FText::FromString(TextoMunicionArma));   
    } else {
        MunicionArmaHUD = MunicionArma;
        bInicializadaMunicionArma = true;
    }
}

void ADispareitorControladorJugador::ActualizarMunicionPersonajeHUD(int32 MunicionPersonaje) {
    DispareitorHUD = DispareitorHUD ? DispareitorHUD : Cast<ADispareitorHUD>(GetHUD());

    if(DispareitorHUD && DispareitorHUD->PantallaDelPersonaje && DispareitorHUD->PantallaDelPersonaje->MunicionPersonaje) {
        FString TextoMunicionPersonaje = FString::Printf(TEXT("%d"), MunicionPersonaje);
        DispareitorHUD->PantallaDelPersonaje->MunicionPersonaje->SetText(FText::FromString(TextoMunicionPersonaje));   
    } else {
        MunicionPersonajeHUD = MunicionPersonaje; 
        bInicializadaMunicionPersonaje = true;
    }
}

void ADispareitorControladorJugador::ActualizarTiempoPartidaHUD(float TiempoCuentaAtras) {
    DispareitorHUD = DispareitorHUD ? DispareitorHUD : Cast<ADispareitorHUD>(GetHUD());

    if(DispareitorHUD && DispareitorHUD->PantallaDelPersonaje && DispareitorHUD->PantallaDelPersonaje->Tiempo) {
        if(TiempoCuentaAtras >= 0.f) {  
            int32 Minutos = FMath::FloorToInt(TiempoCuentaAtras / 60.f);
            int32 Segundos = TiempoCuentaAtras - Minutos * 60;
            FString PartidaTiempoTexto = FString::Printf(TEXT("%02d:%02d"), Minutos, Segundos);
            DispareitorHUD->PantallaDelPersonaje->Tiempo->SetText(FText::FromString(PartidaTiempoTexto)); 
        } else {
            DispareitorHUD->PantallaDelPersonaje->Tiempo->SetText(FText()); 
        }
    }
}

void ADispareitorControladorJugador::ActualizarTiempoAnunciosHUD(float TiempoCuentaAtras) {
    DispareitorHUD = DispareitorHUD ? DispareitorHUD : Cast<ADispareitorHUD>(GetHUD());

    if(DispareitorHUD && DispareitorHUD->AnunciosWidget && DispareitorHUD->AnunciosWidget->TiempoCalentamiento) {
        if(TiempoCuentaAtras >= 0.f) {            
            int32 Minutos = FMath::FloorToInt(TiempoCuentaAtras / 60.f);
            int32 Segundos = TiempoCuentaAtras - Minutos * 60;
            FString TextoTiempoCalentamiento = FString::Printf(TEXT("%02d:%02d"), Minutos, Segundos);
            DispareitorHUD->AnunciosWidget->TiempoCalentamiento->SetText(FText::FromString(TextoTiempoCalentamiento));   
        } else {
            DispareitorHUD->AnunciosWidget->TiempoCalentamiento->SetText(FText());   
        }
    }
}

void ADispareitorControladorJugador::ActualizarCrucetaFrancotiradorHUD(bool bEstaApuntando) {
    DispareitorHUD = DispareitorHUD ? DispareitorHUD : Cast<ADispareitorHUD>(GetHUD());
 
	if (!DispareitorHUD->FrancotiradorCruceta) {
		DispareitorHUD->MostrarFrancotiradorCruceta();
	}
 
	if(DispareitorHUD && DispareitorHUD->FrancotiradorCruceta && DispareitorHUD->FrancotiradorCruceta->AnimacionZoomIn) {
		if (bEstaApuntando) {
			DispareitorHUD->FrancotiradorCruceta->PlayAnimation(DispareitorHUD->FrancotiradorCruceta->AnimacionZoomIn);
		} else {
			DispareitorHUD->FrancotiradorCruceta->PlayAnimation(DispareitorHUD->FrancotiradorCruceta->AnimacionZoomIn, 0.f, 1, EUMGSequencePlayMode::Reverse);
		}
	}
}

void ADispareitorControladorJugador::ActivarTiempoHUD() {
    float TiempoRestante = 0.f;
    if(EstadoPartida == MatchState::WaitingToStart) {
        TiempoRestante = TiempoCalentamiento - ObtenerTiempoServidor() + TiempoInicioNivel;
    } else if(EstadoPartida == MatchState::InProgress) {
        TiempoRestante = TiempoCalentamiento + TiempoPartida - ObtenerTiempoServidor() + TiempoInicioNivel;
    } else if(EstadoPartida == MatchState::Enfriamiento) {
        TiempoRestante = TiempoCalentamiento + TiempoPartida + TiempoEnfriamiento - ObtenerTiempoServidor() + TiempoInicioNivel;
    }
    uint32 SegundosRestantesTemporal = FMath::CeilToInt(TiempoRestante);

    if(HasAuthority()) {
        DispareitorModoJuego = DispareitorModoJuego ? DispareitorModoJuego : Cast<ADispareitorModoJuego>(UGameplayStatics::GetGameMode(this));
        if(DispareitorModoJuego) {
            SegundosRestantesTemporal = FMath::CeilToInt(DispareitorModoJuego->ObtenerTiempoCuentaAtras() + TiempoInicioNivel);
        }   
    }

    if(SegundosRestantesTemporal != SegundosRestantes) {
        if(EstadoPartida == MatchState::WaitingToStart || EstadoPartida == MatchState::Enfriamiento) {
            ActualizarTiempoAnunciosHUD(TiempoRestante);
        } else if(EstadoPartida == MatchState::InProgress) {
            ActualizarTiempoPartidaHUD(TiempoRestante);
        }   
    }
    SegundosRestantes = SegundosRestantesTemporal;
}

// Peticion tiempo (ejecutada en servidor) cliente -> servidor 
void ADispareitorControladorJugador::PedirTiempoServidor_EnServidor_Implementation(float TiempoClientePeticion) {
    float TiempoServidorAlRecibirPeticion = GetWorld()->GetTimeSeconds();
    DevolverTiempoServidor_EnCliente(TiempoClientePeticion, TiempoServidorAlRecibirPeticion);
}

// Respuesta tiempo (ejecutada en cliente) servidor -> cliente
void ADispareitorControladorJugador::DevolverTiempoServidor_EnCliente_Implementation(float TiempoClientePeticion, float TiempoServidorAlRecibirPeticion) {
    // Round Trip Time
    float RTT = GetWorld()->GetTimeSeconds() - TiempoClientePeticion;
    STT = 0.5f * RTT;
    float TiempoServidorActual = TiempoServidorAlRecibirPeticion + (0.5f * RTT);
    DeltaTiempoServidorCliente = TiempoServidorActual - GetWorld()->GetTimeSeconds();
}

float ADispareitorControladorJugador::ObtenerTiempoServidor() {
    return HasAuthority() ? GetWorld()->GetTimeSeconds() : GetWorld()->GetTimeSeconds() + DeltaTiempoServidorCliente;
}

// El momento mas temprano en el que podemos sincronizar con el servidor
void ADispareitorControladorJugador::ReceivedPlayer() {
    Super::ReceivedPlayer();
    if(IsLocalController()) {
        PedirTiempoServidor_EnServidor(GetWorld()->GetTimeSeconds());
    }
}

void ADispareitorControladorJugador::ComprobarTiempoSincronizacion(float DeltaTime) {
    TiempoSincronizacionPasado += DeltaTime;
    if(IsLocalController() && TiempoSincronizacionPasado > TiempoSincronizacionFrecuencia) {
        PedirTiempoServidor_EnServidor(GetWorld()->GetTimeSeconds());
        TiempoSincronizacionPasado = 0.f;
    }
}

// Llamado por ADispareitorModoJuego::OnMatchStateSet
void ADispareitorControladorJugador::ActualizarEstadoPartida(FName Estado,  bool bPartidaPorEquipos) {
    EstadoPartida = Estado;
    if(EstadoPartida == MatchState::InProgress) {
        ManejarEstadoPartidaHaEmpezado(bPartidaPorEquipos);
    }  else if(EstadoPartida == MatchState::Enfriamiento) {
        ManejarEstadoPartidaEnfriamiento();
    }
}

void ADispareitorControladorJugador::AlReplicar_EstadoPartida() {
    if(EstadoPartida == MatchState::InProgress) {
        ManejarEstadoPartidaHaEmpezado();
    }  else if(EstadoPartida == MatchState::Enfriamiento) {
        ManejarEstadoPartidaEnfriamiento();
    }
}

void ADispareitorControladorJugador::ManejarEstadoPartidaHaEmpezado(bool bPartidaPorEquipos) {
    if(HasAuthority()) {
        bMostrarPuntuacionEquipos = bPartidaPorEquipos;
    }

    DispareitorHUD = DispareitorHUD ? DispareitorHUD : Cast<ADispareitorHUD>(GetHUD());
    if(DispareitorHUD) {        
        if(!DispareitorHUD->PantallaDelPersonaje) {
            DispareitorHUD->MostrarPantallaDelPersonaje();
        }
        if(DispareitorHUD->AnunciosWidget) {
            DispareitorHUD->AnunciosWidget->SetVisibility(ESlateVisibility::Hidden);
        }
        if(HasAuthority()) {
            if(bPartidaPorEquipos) {
                InicializarPuntuacionEquipos();
            } else {
                EsconderPuntuacionEquipos();
            }
        }            
    }
}

void ADispareitorControladorJugador::ManejarEstadoPartidaEnfriamiento() {    
    DispareitorHUD = DispareitorHUD ? DispareitorHUD : Cast<ADispareitorHUD>(GetHUD());
    if(DispareitorHUD) {        
        DispareitorHUD->PantallaDelPersonaje->RemoveFromParent();
        if(DispareitorHUD->AnunciosWidget && DispareitorHUD->AnunciosWidget->PartidaComienza && DispareitorHUD->AnunciosWidget->Informacion) {
            DispareitorHUD->AnunciosWidget->SetVisibility(ESlateVisibility::Visible);
                
            FString TextoPartidaComienza = Anuncio::NuevaPartidaComienzaEn;
            DispareitorHUD->AnunciosWidget->PartidaComienza->SetText(FText::FromString(TextoPartidaComienza));
                
            ADispareitorEstadoJuego* DEstadoJuego = Cast<ADispareitorEstadoJuego>(UGameplayStatics::GetGameState(this));
            if(DEstadoJuego) {
                FString AnuncioGanador = bMostrarPuntuacionEquipos ? ObtenerAnuncioEquipos(DEstadoJuego) : ObtenerAnuncio(DEstadoJuego->ArrayDeEstadoJugadoresConPuntuacionMasAlta);                    
                DispareitorHUD->AnunciosWidget->Informacion->SetText(FText::FromString(AnuncioGanador));
            }
        }
    } 
    
    ADispareitorPersonaje* DispareitorPersonaje = Cast<ADispareitorPersonaje>(GetPawn());
    if(DispareitorPersonaje && DispareitorPersonaje->ObtenerCombateComponente()) {
        DispareitorPersonaje->bSoloGirarCamara = true;
        DispareitorPersonaje->ObtenerCombateComponente()->DispararPresionado(false);
    }    
}

void ADispareitorControladorJugador::ComprobarPingAlto(float DeltaTime) {
    TiempoParaSiguienteComprobacionPingAlto += DeltaTime;
    if(TiempoParaSiguienteComprobacionPingAlto > FrecuenciaChequeoPingAlto) {
        // PlayerState = PlayerState != nullptr ? PlayerState : GetPlayerState<APlayerState>(); ESTA LINEA FALLA EN LINUX AL COMPILAR AUNQUE DA LA IMPRESION DE QUE NO ES NECESARIA
        if(PlayerState) {
            if(PlayerState->GetCompressedPing() * 4 > UmbralPingAlto) { // ping se guarda comprimido y dividido por 4
                IniciarAnimacionPingAlto();
                TiempoEjecutandoseAnimacionPingAlto = 0.f;
                ReportarEstadoPing_EnServidor(true);
            } else {
                ReportarEstadoPing_EnServidor(false);
            }
        }
        TiempoParaSiguienteComprobacionPingAlto = 0.f;
    }
    if(DispareitorHUD && DispareitorHUD->PantallaDelPersonaje && DispareitorHUD->PantallaDelPersonaje->AnimacionPingAlto && DispareitorHUD->PantallaDelPersonaje->IsAnimationPlaying(DispareitorHUD->PantallaDelPersonaje->AnimacionPingAlto)) {
       TiempoEjecutandoseAnimacionPingAlto += DeltaTime;
       if(TiempoEjecutandoseAnimacionPingAlto > DuracionAnimacionPingAlto) {
            PararAnimacionPingAlto();
       }
    }
}

void ADispareitorControladorJugador::ReportarEstadoPing_EnServidor_Implementation(bool bPingAlto) {
    DelegadoPingAlto.Broadcast(bPingAlto);
}

void ADispareitorControladorJugador::IniciarAnimacionPingAlto() {
    DispareitorHUD = DispareitorHUD ? DispareitorHUD : Cast<ADispareitorHUD>(GetHUD());

    if(DispareitorHUD && DispareitorHUD->PantallaDelPersonaje && DispareitorHUD->PantallaDelPersonaje->ImagenPingAlto && DispareitorHUD->PantallaDelPersonaje->AnimacionPingAlto) {
        DispareitorHUD->PantallaDelPersonaje->ImagenPingAlto->SetOpacity(1.f);
        DispareitorHUD->PantallaDelPersonaje->PlayAnimation(DispareitorHUD->PantallaDelPersonaje->AnimacionPingAlto, 0.f, 5);
    } 
}

void ADispareitorControladorJugador::PararAnimacionPingAlto() {
    DispareitorHUD = DispareitorHUD ? DispareitorHUD : Cast<ADispareitorHUD>(GetHUD());

    if(DispareitorHUD && DispareitorHUD->PantallaDelPersonaje && DispareitorHUD->PantallaDelPersonaje->ImagenPingAlto && DispareitorHUD->PantallaDelPersonaje->AnimacionPingAlto) {
        DispareitorHUD->PantallaDelPersonaje->ImagenPingAlto->SetOpacity(0.f);
        if(DispareitorHUD->PantallaDelPersonaje->IsAnimationPlaying(DispareitorHUD->PantallaDelPersonaje->AnimacionPingAlto)) {
            DispareitorHUD->PantallaDelPersonaje->StopAnimation(DispareitorHUD->PantallaDelPersonaje->AnimacionPingAlto);
        }
    } 
}

void ADispareitorControladorJugador::MostrarRegresarAMenuPrincipal() {
    if(!ClaseRegresarAMenuPrincipal) {
        return;
    }

    if(!RegresarAMenuPrincipal) {
        RegresarAMenuPrincipal = CreateWidget<URegresarAMenuPrincipal>(this, ClaseRegresarAMenuPrincipal);
    }
    if(RegresarAMenuPrincipal) { 
        bRegresarAMenuPrincipal = !bRegresarAMenuPrincipal; 
        if(bRegresarAMenuPrincipal) {
            RegresarAMenuPrincipal->ActivarMenu();
        } else {
            RegresarAMenuPrincipal->DesactivarMenu();
        }
    }
}

void ADispareitorControladorJugador::AnunciarEliminacion(APlayerState* EstadoJugadorGanador, APlayerState* EstadoJugadorPerdedor) {
    AnunciarEliminacion_EnCliente(EstadoJugadorGanador, EstadoJugadorPerdedor);
}

void ADispareitorControladorJugador::AnunciarEliminacion_EnCliente_Implementation(APlayerState* EstadoJugadorGanador, APlayerState* EstadoJugadorPerdedor) {
    DispareitorHUD = DispareitorHUD ? DispareitorHUD : Cast<ADispareitorHUD>(GetHUD());
    if(DispareitorHUD) {
        DispareitorHUD->MostrarAnunciosEliminacion(EstadoJugadorGanador->GetPlayerName(), EstadoJugadorPerdedor->GetPlayerName());
    }
}

void ADispareitorControladorJugador::EsconderPuntuacionEquipos() {
    DispareitorHUD = DispareitorHUD ? DispareitorHUD : Cast<ADispareitorHUD>(GetHUD());

    if(DispareitorHUD && DispareitorHUD->PantallaDelPersonaje && DispareitorHUD->PantallaDelPersonaje->PuntuacionEquipoAzul && DispareitorHUD->PantallaDelPersonaje->PuntuacionEquipoRojo) {
        DispareitorHUD->PantallaDelPersonaje->PuntuacionEquipoRojo->SetText(FText());
        DispareitorHUD->PantallaDelPersonaje->PuntuacionEquipoAzul->SetText(FText());
    } 
}

void ADispareitorControladorJugador::InicializarPuntuacionEquipos() {
    DispareitorHUD = DispareitorHUD ? DispareitorHUD : Cast<ADispareitorHUD>(GetHUD());

    if(DispareitorHUD && DispareitorHUD->PantallaDelPersonaje && DispareitorHUD->PantallaDelPersonaje->PuntuacionEquipoAzul && DispareitorHUD->PantallaDelPersonaje->PuntuacionEquipoRojo) {
        FString Cero("0");
        DispareitorHUD->PantallaDelPersonaje->PuntuacionEquipoRojo->SetText(FText::FromString(Cero));
        DispareitorHUD->PantallaDelPersonaje->PuntuacionEquipoAzul->SetText(FText::FromString(Cero));
    } 
}

void ADispareitorControladorJugador::ActualizarPuntuacionEquipoRojoHUD(int32 Puntuacion) {
    DispareitorHUD = DispareitorHUD ? DispareitorHUD : Cast<ADispareitorHUD>(GetHUD());

    if(DispareitorHUD && DispareitorHUD->PantallaDelPersonaje && DispareitorHUD->PantallaDelPersonaje->PuntuacionEquipoRojo) {
        FString PuntuacionTexto = FString::Printf(TEXT("%d"), Puntuacion);
        DispareitorHUD->PantallaDelPersonaje->PuntuacionEquipoRojo->SetText(FText::FromString(PuntuacionTexto));
    } 
}
	
void ADispareitorControladorJugador::ActualizarPuntuacionEquipoAzulHUD(int32 Puntuacion) {
    DispareitorHUD = DispareitorHUD ? DispareitorHUD : Cast<ADispareitorHUD>(GetHUD());

    if(DispareitorHUD && DispareitorHUD->PantallaDelPersonaje && DispareitorHUD->PantallaDelPersonaje->PuntuacionEquipoAzul) {
        FString PuntuacionTexto = FString::Printf(TEXT("%d"), Puntuacion);
        DispareitorHUD->PantallaDelPersonaje->PuntuacionEquipoAzul->SetText(FText::FromString(PuntuacionTexto));
    } 
}

void ADispareitorControladorJugador::AlReplicar_MostrarPuntuacionEquipos() {
    if(bMostrarPuntuacionEquipos) {
        InicializarPuntuacionEquipos();
    } else {
        EsconderPuntuacionEquipos();
    }
}

FString ADispareitorControladorJugador::ObtenerAnuncio(const TArray<class ADispareitorEstadoJugador*>& DEstadoJugadoresPuntuacionMasAlta) {
    FString AnuncioTexto;
    ADispareitorEstadoJugador* DEstadoJugador = GetPlayerState<ADispareitorEstadoJugador>();

    if(!DEstadoJugador) {
        return FString();
    }

    if(DEstadoJugadoresPuntuacionMasAlta.Num() == 0) {
        AnuncioTexto = Anuncio::NoHayGanador; 
    } else if(DEstadoJugadoresPuntuacionMasAlta.Num() == 1) {
        if(DEstadoJugadoresPuntuacionMasAlta[0] == DEstadoJugador) {
            AnuncioTexto = Anuncio::EresElGanador;
        } else { 
            AnuncioTexto = FString::Printf(TEXT("%s %s"), *Anuncio::TomaNota, *DEstadoJugadoresPuntuacionMasAlta[0]->GetPlayerName());
        }
    } else if(DEstadoJugadoresPuntuacionMasAlta.Num() > 1) {
        AnuncioTexto = Anuncio::HanQuedadoEmpatados;
        for(auto EstadoJugador : DEstadoJugadoresPuntuacionMasAlta) {
            AnuncioTexto.Append(FString::Printf(TEXT("%s\n"), *EstadoJugador->GetPlayerName()));    
        }
    }    

    return AnuncioTexto;
}

FString ADispareitorControladorJugador::ObtenerAnuncioEquipos(ADispareitorEstadoJuego* DEstadoJuego) {
    FString AnuncioTexto;  

    UE_LOG(LogTemp, Warning, TEXT("ObtenerAnuncioEquipos1"));  

    if(!DEstadoJuego) {
        return FString();
    }

    const int32 PuntuacionEquipoRojo = DEstadoJuego->PuntuacionEquipoRojo;
    const int32 PuntuacionEquipoAzul = DEstadoJuego->PuntuacionEquipoAzul;

    if(PuntuacionEquipoRojo == 0 && PuntuacionEquipoAzul == 0) {
        AnuncioTexto = Anuncio::NoHayGanador; 
    } else if(PuntuacionEquipoRojo == PuntuacionEquipoAzul) {
        AnuncioTexto = FString::Printf(TEXT("%s\n%s y %s"), *Anuncio::EquipoosHanQuedadoEmpatados, *Anuncio::EquipoRojo, *Anuncio::EquipoAzul);
    } else if(PuntuacionEquipoRojo > PuntuacionEquipoAzul) {
        AnuncioTexto = FString::Printf(TEXT("%s\n%s: %d  %s: %d"), *Anuncio::EquipoRojoGano, *Anuncio::EquipoRojo, PuntuacionEquipoRojo, *Anuncio::EquipoAzul, PuntuacionEquipoAzul);
    } else if(PuntuacionEquipoAzul > PuntuacionEquipoRojo) {
        AnuncioTexto = FString::Printf(TEXT("%s\n%s: %d  %s: %d"), *Anuncio::EquipoAzulGano, *Anuncio::EquipoAzul, PuntuacionEquipoAzul, *Anuncio::EquipoRojo, PuntuacionEquipoRojo);
    }   

    return AnuncioTexto;
}
