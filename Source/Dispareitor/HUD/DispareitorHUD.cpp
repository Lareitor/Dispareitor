#include "DispareitorHUD.h"

// Llamado cada frame
void ADispareitorHUD::DrawHUD() {
    Super::DrawHUD();

    FVector2D PantallaTamano;
    if(GEngine) {
        GEngine->GameViewport->GetViewportSize(PantallaTamano);
        const FVector2D PantallaCentro(PantallaTamano.X / 2.f, PantallaTamano.Y / 2.f);

        float AperturaEscalada = CrucetaAperturaMaxima * HUDTexturas.CrucetaApertura;    

        if(HUDTexturas.CrucetaCentro) {
            FVector2D Apertura(0.f, 0.f);
            DibujarCruceta(HUDTexturas.CrucetaCentro, PantallaCentro, Apertura, HUDTexturas.CrucetaColor);
        }
        if(HUDTexturas.CrucetaIzquierda) {
            FVector2D Apertura(-AperturaEscalada, 0.f);
            DibujarCruceta(HUDTexturas.CrucetaIzquierda, PantallaCentro, Apertura, HUDTexturas.CrucetaColor);
        }
        if(HUDTexturas.CrucetaDerecha) {
            FVector2D Apertura(AperturaEscalada, 0.f);
            DibujarCruceta(HUDTexturas.CrucetaDerecha, PantallaCentro, Apertura, HUDTexturas.CrucetaColor);
        }
        if(HUDTexturas.CrucetaArriba) {
            FVector2D Apertura(0.f, -AperturaEscalada);
            DibujarCruceta(HUDTexturas.CrucetaArriba, PantallaCentro, Apertura, HUDTexturas.CrucetaColor);
        }
        if(HUDTexturas.CrucetaAbajo) {
            FVector2D Apertura(0.f, AperturaEscalada);
            DibujarCruceta(HUDTexturas.CrucetaAbajo, PantallaCentro, Apertura, HUDTexturas.CrucetaColor);
        }
    }
}

void ADispareitorHUD::DibujarCruceta(UTexture2D* Textura, FVector2D PantallaCentro, FVector2D Apertura, FLinearColor CrucetaColor) {
    const float TexturaAnchura = Textura->GetSizeX();
    const float TexturaAltura = Textura->GetSizeY();
    const FVector2D TexturaPuntoDondeDibujar(PantallaCentro.X - (TexturaAnchura / 2.f) + Apertura.X, PantallaCentro.Y - (TexturaAltura / 2.f) + Apertura.Y);

    DrawTexture(Textura, TexturaPuntoDondeDibujar.X, TexturaPuntoDondeDibujar.Y, TexturaAnchura, TexturaAltura, 0.f, 0.f, 1.f, 1.f, CrucetaColor);
}


