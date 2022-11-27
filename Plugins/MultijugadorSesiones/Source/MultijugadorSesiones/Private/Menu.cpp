#include "Menu.h"
#include "Components/Button.h"
#include "OnlineSubsystem.h"
#include "SubsistemaInstanciaJuego.h"

void UMenu::MenuConfiguracion(int32 _NumeroDeConexiones, FString  _ModoJuego, FString _PathSalaEspera) {
	NumeroDeConexiones = _NumeroDeConexiones;
	ModoJuego = _ModoJuego;
	PathSalaEspera = FString::Printf(TEXT("%s?listen"), *_PathSalaEspera);

	AddToViewport();
	SetVisibility(ESlateVisibility::Visible);
	bIsFocusable = true;

	UWorld* Mundo = GetWorld();
	if (Mundo) {
		APlayerController* ControladorJugador = Mundo->GetFirstPlayerController();
		if (ControladorJugador) {
			FInputModeUIOnly ModoEntradaDatosSoloUI;
			ModoEntradaDatosSoloUI.SetWidgetToFocus(TakeWidget());
			ModoEntradaDatosSoloUI.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
			ControladorJugador->SetInputMode(ModoEntradaDatosSoloUI);
			ControladorJugador->SetShowMouseCursor(true);
		}
	}

	// UGameInstance crea e inicializa automáticamente todas sus clases derivadas y USubsistemaInstanciaJuego es una de ellas
	UGameInstance* InstanciaJuego = GetGameInstance();
	if (InstanciaJuego) {
		SubsistemaInstanciaJuego = InstanciaJuego->GetSubsystem<USubsistemaInstanciaJuego>();
	}

	if (SubsistemaInstanciaJuego) {
		// A los delegados que son DYNAMIC usamos AddDynamic, a los que no AddUObject
		SubsistemaInstanciaJuego->DelegadoMultijugadorCompletadoCrearSesion.AddDynamic(this, &ThisClass::CallbackCompletadoCrearSesion);
		SubsistemaInstanciaJuego->DelegadoMultijugadorCompletadoEncontrarSesiones.AddUObject(this, &ThisClass::CallbackCompletadoEncontrarSesiones);
		SubsistemaInstanciaJuego->DelegadoMultijugadorCompletadoUnirSesion.AddUObject(this, &ThisClass::CallbackCompletadoUnirSesion);
		SubsistemaInstanciaJuego->DelegadoMultijugadorCompletadoDestruirSesion.AddDynamic(this, &ThisClass::CallbackCompletadoDestruirSesion);
		SubsistemaInstanciaJuego->DelegadoMultijugadorCompletadoEmpezarSesion.AddDynamic(this, &ThisClass::CallbackCompletadoEmpezarSesion);
	}
}

// Es como el constructor del widget
bool UMenu::Initialize() {
	if (!Super::Initialize()) {
		return false;
	}

	if (BotonAnfitrion) {
		// OnClicked es como un delegado
		BotonAnfitrion->OnClicked.AddDynamic(this, &ThisClass::BotonAnfitrionPulsado);
	}

	if (BotonUnirse) {		
		BotonUnirse->OnClicked.AddDynamic(this, &ThisClass::BotonUnirsePulsado);
	}

	return true;
}

// Se llama cuando el nivel en el que está es destruido
// Esto ocurrirá cuando vayamos al nivel de espera
void UMenu::NativeDestruct() {
	MenuDeshacerConfiguracion();
	Super::NativeDestruct();
}


void UMenu::CallbackCompletadoCrearSesion(bool fueOk) {
	if (fueOk) {
		if (GEngine) {
			GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Yellow, FString(TEXT("Sesion creado correctamente!")));
		}

		UWorld* Mundo = GetWorld();
		if (Mundo) {
			Mundo->ServerTravel(PathSalaEspera);
		}
	} else {
		if (GEngine) {
			GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Red, FString(TEXT("Error al crear la sesion!")));
		}
		BotonAnfitrion->SetIsEnabled(true);

	}
}

void UMenu::CallbackCompletadoEncontrarSesiones(const TArray<FOnlineSessionSearchResult>& ResultadoBusquedaSesion, bool fueOk) {
	if (SubsistemaInstanciaJuego == nullptr) {
		return;
	}

	for (auto Resultado : ResultadoBusquedaSesion) {
		FString ModoJuegoEncontrado;
		Resultado.Session.SessionSettings.Get(FName("ModoJuego"), ModoJuegoEncontrado);
		if (ModoJuegoEncontrado == ModoJuego) {
			SubsistemaInstanciaJuego->UnirSesion(Resultado);
			return;
		}
	}

	if (GEngine) {
		GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Red, FString(TEXT("No se ha encontrado ningun modo juego: " + ModoJuego)));
	}

	if (!fueOk || ResultadoBusquedaSesion.Num() == 0) {
		BotonUnirse->SetIsEnabled(true);
	}
}

void UMenu::CallbackCompletadoUnirSesion(EOnJoinSessionCompleteResult::Type Resultado) {
	IOnlineSubsystem* SubsistemaOnline = IOnlineSubsystem::Get();
	if (SubsistemaOnline) {
		IOnlineSessionPtr InterfazSesion = SubsistemaOnline->GetSessionInterface();
		if (InterfazSesion.IsValid()) {
			FString IP;
			InterfazSesion->GetResolvedConnectString(NAME_GameSession, IP);
			
			APlayerController* ControladorJugador = GetGameInstance()->GetFirstLocalPlayerController();
			if (ControladorJugador) {
				ControladorJugador->ClientTravel(IP, ETravelType::TRAVEL_Absolute);
			}
		}
	}

	if (Resultado != EOnJoinSessionCompleteResult::Success) {
		BotonUnirse->SetIsEnabled(true);
	}
}

void UMenu::CallbackCompletadoDestruirSesion(bool fueOk) {
}

void UMenu::CallbackCompletadoEmpezarSesion(bool fueOk) {
}

void UMenu::BotonAnfitrionPulsado() {	
	BotonAnfitrion->SetIsEnabled(false);

	if (SubsistemaInstanciaJuego) {
		SubsistemaInstanciaJuego->CrearSesion(NumeroDeConexiones, ModoJuego);		
	}
}

void UMenu::BotonUnirsePulsado() {
	BotonUnirse->SetIsEnabled(false);

	if (SubsistemaInstanciaJuego) {
		SubsistemaInstanciaJuego->EncontrarSesiones(10000);
	}
}

void UMenu::MenuDeshacerConfiguracion() {
	RemoveFromParent();
	UWorld* Mundo = GetWorld();
	if (Mundo) {
		APlayerController* ControladorJugador = Mundo->GetFirstPlayerController();
		if (ControladorJugador) {
			FInputModeGameOnly ModoEntradaDatosSoloJuego;
			ControladorJugador->SetInputMode(ModoEntradaDatosSoloJuego);
			ControladorJugador->SetShowMouseCursor(false);
		}
	}
}
