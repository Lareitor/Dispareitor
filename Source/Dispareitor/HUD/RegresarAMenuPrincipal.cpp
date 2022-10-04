#include "RegresarAMenuPrincipal.h"
#include "GameFramework/PlayerController.h"
#include "Components/Button.h"
#include "SubsistemaInstanciaJuego.h"
#include "GameFramework/GameModeBase.h"


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

    if(BotonMenuPrincipal) {
        BotonMenuPrincipal->OnClicked.AddDynamic(this, &URegresarAMenuPrincipal::PulsadoBotonMenuPrincipal);
    }

    UGameInstance* InstanciaJuego = GetGameInstance();
    if(InstanciaJuego) {
        SubsistemaInstanciaJuego = InstanciaJuego->GetSubsystem<USubsistemaInstanciaJuego>();
        if(SubsistemaInstanciaJuego) {
            SubsistemaInstanciaJuego->DelegadoMultijugadorCompletadoDestruirSesion.AddDynamic(this, &URegresarAMenuPrincipal::Callback_AlDestruirSesion);
        }
    }
}

bool URegresarAMenuPrincipal::Initialize() {
    if(!Super::Initialize()) {
        return false;
    }

    return true;
}

void URegresarAMenuPrincipal::Callback_AlDestruirSesion(bool bFueOk) {
    if(!bFueOk) {
        BotonMenuPrincipal->SetIsEnabled(true);
        return;
    }

    UWorld* Mundo = GetWorld();
    if(Mundo) {
        AGameModeBase* ModoJuego = Mundo->GetAuthGameMode<AGameModeBase>();
        if(ModoJuego) { // Estamos en el servidor
            ModoJuego->ReturnToMainMenuHost();
        } else { // Estamos en el cliente
            ControladorJugador = ControladorJugador ? ControladorJugador : Mundo->GetFirstPlayerController();
            if(ControladorJugador) {
                ControladorJugador->ClientReturnToMainMenuWithTextReason(FText());
            }
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
}

void URegresarAMenuPrincipal::PulsadoBotonMenuPrincipal() {
    BotonMenuPrincipal->SetIsEnabled(false);

    if(SubsistemaInstanciaJuego) {
        SubsistemaInstanciaJuego->DestruirSesion();
    }
}


