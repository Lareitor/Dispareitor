#include "DispareitorControladorJugador.h"
#include "Dispareitor/HUD/DispareitorHUD.h"
#include "Dispareitor/HUD/PantallaDelPersonaje.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Dispareitor/Personaje/DispareitorPersonaje.h"

void ADispareitorControladorJugador::BeginPlay() {
    Super::BeginPlay();

    DispareitorHUD = Cast<ADispareitorHUD>(GetHUD());
}

void ADispareitorControladorJugador::OnPossess(APawn* Peon) {
    Super::OnPossess(Peon);

    ADispareitorPersonaje* DispareitorPersonaje = Cast<ADispareitorPersonaje>(Peon);
    if(DispareitorPersonaje) {
        ActualizarHUDVida(DispareitorPersonaje->ObtenerVida(), DispareitorPersonaje->ObtenerVidaMaxima());
    }
}

// Llamado por DispareitorPersonaje
void ADispareitorControladorJugador::ActualizarHUDVida(float Vida, float VidaMaxima) {
    DispareitorHUD = DispareitorHUD != nullptr ? DispareitorHUD : Cast<ADispareitorHUD>(GetHUD());

    if(DispareitorHUD && DispareitorHUD->PantallaDelPersonaje && DispareitorHUD->PantallaDelPersonaje->VidaBarra && DispareitorHUD->PantallaDelPersonaje->VidaTexto) {
        DispareitorHUD->PantallaDelPersonaje->VidaBarra->SetPercent(Vida / VidaMaxima); 
        FString VidaTexto = FString::Printf(TEXT("%d/%d"), FMath::CeilToInt(Vida), FMath::CeilToInt(VidaMaxima));
        DispareitorHUD->PantallaDelPersonaje->VidaTexto->SetText(FText::FromString(VidaTexto)); 
    }
}

void ADispareitorControladorJugador::ActualizarHUDMuertos(float Muertos) {
    DispareitorHUD = DispareitorHUD != nullptr ? DispareitorHUD : Cast<ADispareitorHUD>(GetHUD());

    if(DispareitorHUD && DispareitorHUD->PantallaDelPersonaje && DispareitorHUD->PantallaDelPersonaje->Muertos) {
        FString MuertosTexto = FString::Printf(TEXT("%d"), FMath::FloorToInt(Muertos));
        DispareitorHUD->PantallaDelPersonaje->Muertos->SetText(FText::FromString(MuertosTexto));   
    }
}

void ADispareitorControladorJugador::ActualizarHUDMuertes(int32 Muertes) {
    DispareitorHUD = DispareitorHUD != nullptr ? DispareitorHUD : Cast<ADispareitorHUD>(GetHUD());

    if(DispareitorHUD && DispareitorHUD->PantallaDelPersonaje && DispareitorHUD->PantallaDelPersonaje->Muertes) {
        FString MuertesTexto = FString::Printf(TEXT("%d"), Muertes);
        DispareitorHUD->PantallaDelPersonaje->Muertes->SetText(FText::FromString(MuertesTexto));   
    }
}


