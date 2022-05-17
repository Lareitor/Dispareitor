#include "DispareitorHUD.h"

// Llamada cada frame
void ADispareitorHUD::DrawHUD() {
    Super::DrawHUD();

    FVector2D PantallaTamano;
    if(GEngine) {
        GEngine->GameViewport->GetViewportSize(PantallaTamano);
        const FVector2D PantallaCentro(PantallaTamano.X / 2.f, PantallaTamano.Y / 2.f);

        float AperturaEscalada = CrucetaAperturaMaxima * HUDTexturas.CrucetaApertura;    

        if(HUDTexturas.CrucetaCentro) {
            FVector2D Apertura(0.f, 0.f);
            DibujarCruceta(HUDTexturas.CrucetaCentro, PantallaCentro, Apertura);
        }
        if(HUDTexturas.CrucetaIzquierda) {
            FVector2D Apertura(-AperturaEscalada, 0.f);
            DibujarCruceta(HUDTexturas.CrucetaIzquierda, PantallaCentro, Apertura);
        }
        if(HUDTexturas.CrucetaDerecha) {
            FVector2D Apertura(AperturaEscalada, 0.f);
            DibujarCruceta(HUDTexturas.CrucetaDerecha, PantallaCentro, Apertura);
        }
        if(HUDTexturas.CrucetaArriba) {
            FVector2D Apertura(0.f, -AperturaEscalada);
            DibujarCruceta(HUDTexturas.CrucetaArriba, PantallaCentro, Apertura);
        }
        if(HUDTexturas.CrucetaAbajo) {
            FVector2D Apertura(0.f, AperturaEscalada);
            DibujarCruceta(HUDTexturas.CrucetaAbajo, PantallaCentro, Apertura);
        }
    }
}

void ADispareitorHUD::DibujarCruceta(UTexture2D* Textura, FVector2D PantallaCentro, FVector2D Apertura) {
    const float TexturaAnchura = Textura->GetSizeX();
    const float TexturaAltura = Textura->GetSizeY();
    const FVector2D TexturaPuntoDondeDibujar(PantallaCentro.X - (TexturaAnchura / 2.f) + Apertura.X, PantallaCentro.Y - (TexturaAltura / 2.f) + Apertura.Y);

    DrawTexture(Textura, TexturaPuntoDondeDibujar.X, TexturaPuntoDondeDibujar.Y, TexturaAnchura, TexturaAltura, 0.f, 0.f, 1.f, 1.f, FLinearColor::White);
}


