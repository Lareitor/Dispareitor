#include "DispareitorHUD.h"

// Llamada cada frame
void ADispareitorHUD::DrawHUD() {
    Super::DrawHUD();

    FVector2D PantallaTamano;
    if(GEngine) {
        GEngine->GameViewport->GetViewportSize(PantallaTamano);
        const FVector2D PantallaCentro(PantallaTamano.X / 2.f, PantallaTamano.Y / 2.f);

        if(HUDTexturas.CrucetaCentro) {
            DibujarCruceta(HUDTexturas.CrucetaCentro, PantallaCentro);
        }
        if(HUDTexturas.CrucetaIzquierda) {
            DibujarCruceta(HUDTexturas.CrucetaIzquierda, PantallaCentro);
        }
        if(HUDTexturas.CrucetaDerecha) {
            DibujarCruceta(HUDTexturas.CrucetaDerecha, PantallaCentro);
        }
        if(HUDTexturas.CrucetaArriba) {
            DibujarCruceta(HUDTexturas.CrucetaArriba, PantallaCentro);
        }
        if(HUDTexturas.CrucetaAbajo) {
            DibujarCruceta(HUDTexturas.CrucetaAbajo, PantallaCentro);
        }
    }
}

void ADispareitorHUD::DibujarCruceta(UTexture2D* Textura, FVector2D PantallaCentro) {
    const float TexturaAnchura = Textura->GetSizeX();
    const float TexturaAltura = Textura->GetSizeY();
    const FVector2D TexturaPuntoDondeDibujar(PantallaCentro.X - (TexturaAnchura / 2.f), PantallaCentro.Y - (TexturaAltura / 2.f));

    DrawTexture(Textura, TexturaPuntoDondeDibujar.X, TexturaPuntoDondeDibujar.Y, TexturaAnchura, TexturaAltura, 0.f, 0.f, 1.f, 1.f, FLinearColor::White);
}


