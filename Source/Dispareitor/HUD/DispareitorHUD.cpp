#include "DispareitorHUD.h"
#include "GameFramework/PlayerController.h"
#include "PantallaDelPersonaje.h"
#include "AnunciosWidget.h"
#include "FrancotiradorCruceta.h"
#include "AnunciosEliminacion.h"
#include "Dispareitor/ControladorJugador/DispareitorControladorJugador.h"
#include "Components/HorizontalBox.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/CanvasPanelSlot.h"

void ADispareitorHUD::BeginPlay() {
    Super::BeginPlay();
}

void ADispareitorHUD::MostrarPantallaDelPersonaje() {
    APlayerController* ControladorDeJugador = GetOwningPlayerController();

    if(ControladorDeJugador && ClasePantallaDelPersonaje) {
        PantallaDelPersonaje = CreateWidget<UPantallaDelPersonaje>(ControladorDeJugador, ClasePantallaDelPersonaje);
        PantallaDelPersonaje->AddToViewport();
    }
}

void ADispareitorHUD::MostrarAnunciosWidget() {
    APlayerController* ControladorDeJugador = GetOwningPlayerController();

    if(ControladorDeJugador && ClaseAnunciosWidget) {
        AnunciosWidget = CreateWidget<UAnunciosWidget>(ControladorDeJugador, ClaseAnunciosWidget);
        AnunciosWidget->AddToViewport();
    }
}

void ADispareitorHUD::MostrarFrancotiradorCruceta() {
    APlayerController* ControladorDeJugador = GetOwningPlayerController();

    if(ControladorDeJugador && ClaseFrancotiradorCruceta) {
        FrancotiradorCruceta = CreateWidget<UFrancotiradorCruceta>(ControladorDeJugador, ClaseFrancotiradorCruceta);
        FrancotiradorCruceta->AddToViewport();
    }
}

// Llamado cada frame
void ADispareitorHUD::DrawHUD() {
    Super::DrawHUD();

    FVector2D TamanioPantalla;
    if(GEngine) {
        GEngine->GameViewport->GetViewportSize(TamanioPantalla);
        const FVector2D CentroPantalla(TamanioPantalla.X / 2.f, TamanioPantalla.Y / 2.f);

        float AperturaEscalada = AperturaMaximaCruceta * HUDCruceta.CrucetaApertura;    

        if(HUDCruceta.CrucetaCentro) {
            FVector2D Apertura(0.f, 0.f);
            DibujarCruceta(HUDCruceta.CrucetaCentro, CentroPantalla, Apertura, HUDCruceta.CrucetaColor);
        }
        if(HUDCruceta.CrucetaIzquierda) {
            FVector2D Apertura(-AperturaEscalada, 0.f);
            DibujarCruceta(HUDCruceta.CrucetaIzquierda, CentroPantalla, Apertura, HUDCruceta.CrucetaColor);
        }
        if(HUDCruceta.CrucetaDerecha) {
            FVector2D Apertura(AperturaEscalada, 0.f);
            DibujarCruceta(HUDCruceta.CrucetaDerecha, CentroPantalla, Apertura, HUDCruceta.CrucetaColor);
        }
        if(HUDCruceta.CrucetaArriba) {
            FVector2D Apertura(0.f, -AperturaEscalada);
            DibujarCruceta(HUDCruceta.CrucetaArriba, CentroPantalla, Apertura, HUDCruceta.CrucetaColor);
        }
        if(HUDCruceta.CrucetaAbajo) {
            FVector2D Apertura(0.f, AperturaEscalada);
            DibujarCruceta(HUDCruceta.CrucetaAbajo, CentroPantalla, Apertura, HUDCruceta.CrucetaColor);
        }
    }
}

void ADispareitorHUD::DibujarCruceta(UTexture2D* Textura, FVector2D CentroPantalla, FVector2D Apertura, FLinearColor CrucetaColor) {
    const float AnchuraTextura = Textura->GetSizeX();
    const float AlturaTextura = Textura->GetSizeY();
    const FVector2D PosicionDondeDibujarTextura(CentroPantalla.X - (AnchuraTextura / 2.f) + Apertura.X, CentroPantalla.Y - (AlturaTextura / 2.f) + Apertura.Y);

    DrawTexture(Textura, PosicionDondeDibujarTextura.X, PosicionDondeDibujarTextura.Y, AnchuraTextura, AlturaTextura, 0.f, 0.f, 1.f, 1.f, CrucetaColor);
}

void ADispareitorHUD::MostrarAnunciosEliminacion(FString NombreGanador, FString NombrePerdedor) {
    DispareitorControladorJugador = DispareitorControladorJugador ? DispareitorControladorJugador : Cast<ADispareitorControladorJugador>(GetOwningPlayerController());
    if(DispareitorControladorJugador && ClaseAnunciosEliminacion) {
        UAnunciosEliminacion* AnunciosEliminacion = CreateWidget<UAnunciosEliminacion>(DispareitorControladorJugador, ClaseAnunciosEliminacion);
        if(AnunciosEliminacion) {
            AnunciosEliminacion->ActualizarTextoEliminacion(NombreGanador, NombrePerdedor);
            AnunciosEliminacion->AddToViewport();
            for(UAnunciosEliminacion* AnuncioEliminacion : ArrayAnunciosEliminacion) {
                if(AnuncioEliminacion && AnuncioEliminacion->CajaAnunciosEliminacion) {
                    UCanvasPanelSlot* CanvasPanelSlotDelMensaje = UWidgetLayoutLibrary::SlotAsCanvasSlot(AnuncioEliminacion->CajaAnunciosEliminacion);
                    if(CanvasPanelSlotDelMensaje) {
                        FVector2D PosicionDelMensaje = CanvasPanelSlotDelMensaje->GetPosition();
                        FVector2D NuevaPosicion(PosicionDelMensaje.X, PosicionDelMensaje.Y + CanvasPanelSlotDelMensaje->GetSize().Y);
                        CanvasPanelSlotDelMensaje->SetPosition(NuevaPosicion);
                    }
                    
                }
            }

            ArrayAnunciosEliminacion.Add(AnunciosEliminacion);

            FTimerHandle ManejadorTiempoEliminacion;
            FTimerDelegate DelegadoTiempoEliminacion;
            DelegadoTiempoEliminacion.BindUFunction(this, FName("TiempoAnuncioEliminacionFinalizado"), AnunciosEliminacion);
            GetWorldTimerManager().SetTimer(ManejadorTiempoEliminacion, DelegadoTiempoEliminacion, TiempoAnuncioEliminacion, false);
        }
    }
}

void ADispareitorHUD::TiempoAnuncioEliminacionFinalizado(UAnunciosEliminacion* AnunciosEliminacionAEliminar) {
    if(AnunciosEliminacionAEliminar) {
        AnunciosEliminacionAEliminar->RemoveFromParent();
    }
}

