#include "RegresarAMenuPrincipal.h"
#include "GameFramework/PlayerController.h"
#include "Components/Button.h"
#include "SubsistemaInstanciaJuego.h"
#include "GameFramework/GameModeBase.h"
#include "Dispareitor/Personaje/DispareitorPersonaje.h"


void URegresarAMenuPrincipal::ActivarMenu() {
    AddToViewport();
    SetVisibility(ESlateVisibility::Visible);
    bIsFocusable = true;

    UWorld* Mundo = GetWorld();
    if(Mundo) {
        ControladorJugador = ControladorJugador ? ControladorJugador : Mundo->GetFirstPlayerController();
        if(ControladorJugador) {
            FInputModeGameAndUI EntradaModoJuegoYUI; 
            EntradaModoJuegoYUI.SetWidgetToFocus(TakeWidget());
            ControladorJugador->SetInputMode(EntradaModoJuegoYUI);
            ControladorJugador->SetShowMouseCursor(true);
        }
    }

    if(BotonMenuPrincipal && !BotonMenuPrincipal->OnClicked.IsBound()) {
        BotonMenuPrincipal->OnClicked.AddDynamic(this, &URegresarAMenuPrincipal::Callback_PulsadoBotonMenuPrincipal);
    }

    UGameInstance* InstanciaJuego = GetGameInstance();
    if(InstanciaJuego) {
        SubsistemaInstanciaJuego = InstanciaJuego->GetSubsystem<USubsistemaInstanciaJuego>();
        if(SubsistemaInstanciaJuego) {
            SubsistemaInstanciaJuego->DelegadoMultijugadorCompletadoDestruirSesion.AddDynamic(this, &URegresarAMenuPrincipal::Callback_AlDestruirSesion);
        }
    }
}

void URegresarAMenuPrincipal::DesactivarMenu() {
    RemoveFromParent();

    UWorld* Mundo = GetWorld();
    if(Mundo) {
        ControladorJugador = ControladorJugador ? ControladorJugador : Mundo->GetFirstPlayerController();
        if(ControladorJugador) {
            FInputModeGameOnly EntradaModoJuego; 
            ControladorJugador->SetInputMode(EntradaModoJuego);
            ControladorJugador->SetShowMouseCursor(false);
        }
    }

    if(BotonMenuPrincipal && BotonMenuPrincipal->OnClicked.IsBound()) {
        BotonMenuPrincipal->OnClicked.RemoveDynamic(this, &URegresarAMenuPrincipal::Callback_PulsadoBotonMenuPrincipal);
    }
    if(SubsistemaInstanciaJuego && SubsistemaInstanciaJuego->DelegadoMultijugadorCompletadoDestruirSesion.IsBound()) {
        SubsistemaInstanciaJuego->DelegadoMultijugadorCompletadoDestruirSesion.RemoveDynamic(this, &URegresarAMenuPrincipal::Callback_AlDestruirSesion);
    }
}

bool URegresarAMenuPrincipal::Initialize() {
    if(!Super::Initialize()) {
        return false;
    }

    return true;
}

void URegresarAMenuPrincipal::Callback_PulsadoBotonMenuPrincipal() {
    BotonMenuPrincipal->SetIsEnabled(false);

    UWorld* Mundo = GetWorld();
    if(Mundo) {
        APlayerController* PrimerControladorJugador = Mundo->GetFirstPlayerController();
        if(PrimerControladorJugador) {
            ADispareitorPersonaje* DispareitorPersonaje = Cast<ADispareitorPersonaje>(PrimerControladorJugador->GetPawn());
            if(DispareitorPersonaje) {
                DispareitorPersonaje->DejarJuego_EnServidor();
                DispareitorPersonaje->DelegadoDejarJuego.AddDynamic(this, &URegresarAMenuPrincipal::Callback_AlDejarElJuego);
            } else { // Estamos en mitad de una reaparicion, volvemos a mostrar el boton hasta que tengamos DispareitorPersonaje 
                BotonMenuPrincipal->SetIsEnabled(true);
            }
        }
    }
}

void URegresarAMenuPrincipal::Callback_AlDejarElJuego() {
    if(SubsistemaInstanciaJuego) {
        SubsistemaInstanciaJuego->DestruirSesion();
    }
}

void URegresarAMenuPrincipal::Callback_AlDestruirSesion(bool bFueOk) {
    if(!bFueOk) {
        BotonMenuPrincipal->SetIsEnabled(true);
        return;
    }

    UWorld* Mundo = GetWorld();
    if(Mundo) {
        AGameModeBase* ModoJuego = Mundo->GetAuthGameMode<AGameModeBase>();
        if(ModoJuego) { // Estamos en el servidor ya que este objeto solo existe en el servidor
            ModoJuego->ReturnToMainMenuHost();
        } else { // Estamos en el cliente
            ControladorJugador = ControladorJugador ? ControladorJugador : Mundo->GetFirstPlayerController();
            if(ControladorJugador) {
                ControladorJugador->ClientReturnToMainMenuWithTextReason(FText());
            }
        }
    }
}





