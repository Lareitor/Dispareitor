// Fill out your copyright notice in the Description page of Project Settings.


#include "SubsistemaInstanciaJuego.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"

USubsistemaInstanciaJuego::USubsistemaInstanciaJuego() :
	DelegadoCompletadoCrearSesion(FOnCreateSessionCompleteDelegate::CreateUObject(this, &ThisClass::CallbackCompletadoCrearSesion)),
	DelegadoCompletadoEncontrarSesiones(FOnFindSessionsCompleteDelegate::CreateUObject(this, &ThisClass::CallbackCompletadoEncontrarSesiones)),
	DelegadoCompletadoUnirSesion(FOnJoinSessionCompleteDelegate::CreateUObject(this, &ThisClass::CallbackCompletadoUnirSesion)),
	DelegadoCompletadoDestruirSesion(FOnDestroySessionCompleteDelegate::CreateUObject(this, &ThisClass::CallbackCompletadoDestruirSesion)),
	DelegadoCompletadoEmpezarSesion(FOnStartSessionCompleteDelegate::CreateUObject(this, &ThisClass::CallbackCompletadoEmpezarSesion)) {

	IOnlineSubsystem* SubsistemaOnline = IOnlineSubsystem::Get();
	if (SubsistemaOnline) {
		InterfazSesion = SubsistemaOnline->GetSessionInterface();
	}
}

void USubsistemaInstanciaJuego::CrearSesion(int32 NumeroConexiones, FString ModoJuego) {
	NumeroConexionesDeseadas = NumeroConexiones;
	ModoJuegoDeseado = ModoJuego;
	
	if (!InterfazSesion.IsValid()) {
		return;
	}

	auto ExisteSesion = InterfazSesion->GetNamedSession(NAME_GameSession);
	if (ExisteSesion != nullptr) {
		bCrearSesionAlDestruir = true;
		UltimoNumeroConexiones = NumeroConexiones;
		UltimoModoJuego = ModoJuego;
		DestruirSesion();
	}

	// Almacenar el delegado en un FDelegateHandle para poder borrarlo posteriormente de la lista de delegados
	ManejadorDelegadoCompletadoCrearSesion = InterfazSesion->AddOnCreateSessionCompleteDelegate_Handle(DelegadoCompletadoCrearSesion);

	ConfiguracionUltimaSesion = MakeShareable(new FOnlineSessionSettings());
	ConfiguracionUltimaSesion->bIsLANMatch = IOnlineSubsystem::Get()->GetSubsystemName() == "NULL";
	ConfiguracionUltimaSesion->NumPublicConnections = NumeroConexiones;
	ConfiguracionUltimaSesion->bAllowJoinInProgress = true;
	ConfiguracionUltimaSesion->bAllowJoinViaPresence = true;
	ConfiguracionUltimaSesion->bShouldAdvertise = true;
	ConfiguracionUltimaSesion->bUsesPresence = true;
	ConfiguracionUltimaSesion->Set(FName("ModoJuego"), ModoJuego, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	ConfiguracionUltimaSesion->BuildUniqueId = 1;
	ConfiguracionUltimaSesion->bUseLobbiesIfAvailable = true;

	const ULocalPlayer* JugadorLocal = GetWorld()->GetFirstLocalPlayerFromController();

	// Si devuelve true se ejecutar� USubsistemaInstanciaJuego::CallbackCompletadoCrearSesion
	if (!InterfazSesion->CreateSession(*JugadorLocal->GetPreferredUniqueNetId(), NAME_GameSession, *ConfiguracionUltimaSesion)) {
		InterfazSesion->ClearOnCreateSessionCompleteDelegate_Handle(ManejadorDelegadoCompletadoCrearSesion);

		// Emitir nuestro propio delegado
		DelegadoMultijugadorCompletadoCrearSesion.Broadcast(false);
	}

}

void USubsistemaInstanciaJuego::EncontrarSesiones(int32 NumeroMaximoResultados) {
	if (!InterfazSesion.IsValid()) {
		return;
	}

	ManejadorDelegadoCompletadoEncontrarSesiones =  InterfazSesion->AddOnFindSessionsCompleteDelegate_Handle(DelegadoCompletadoEncontrarSesiones);

	BusquedaUltimaSesion = MakeShareable(new FOnlineSessionSearch());
	BusquedaUltimaSesion->MaxSearchResults = NumeroMaximoResultados;
	BusquedaUltimaSesion->bIsLanQuery = IOnlineSubsystem::Get()->GetSubsystemName() == "NULL";
	BusquedaUltimaSesion->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);

	const ULocalPlayer* JugadorLocal = GetWorld()->GetFirstLocalPlayerFromController();

	// Si devuelve true se ejecutar� USubsistemaInstanciaJuego::CallbackCompletadoEncontrarSesiones 
	if (!InterfazSesion->FindSessions(*JugadorLocal->GetPreferredUniqueNetId(), BusquedaUltimaSesion.ToSharedRef())) {
		InterfazSesion->ClearOnFindSessionsCompleteDelegate_Handle(ManejadorDelegadoCompletadoEncontrarSesiones);

		// Emitir nuestro propio delegado
		DelegadoMultijugadorCompletadoEncontrarSesiones.Broadcast(TArray<FOnlineSessionSearchResult>(), false);
	}
}

void USubsistemaInstanciaJuego::UnirSesion(const FOnlineSessionSearchResult& ResultadoBusquedaSesion) {
	if (!InterfazSesion.IsValid()) {
		DelegadoMultijugadorCompletadoUnirSesion.Broadcast(EOnJoinSessionCompleteResult::UnknownError);
		return;
	}

	ManejadorDelegadoCompletadoUnirSesion = InterfazSesion->AddOnJoinSessionCompleteDelegate_Handle(DelegadoCompletadoUnirSesion);
	const ULocalPlayer* JugadorLocal = GetWorld()->GetFirstLocalPlayerFromController();

	// Si devuelve true se ejecutar� USubsistemaInstanciaJuego::CallbackCompletadoUnirSesion 
	if (!InterfazSesion->JoinSession(*JugadorLocal->GetPreferredUniqueNetId(), NAME_GameSession, ResultadoBusquedaSesion)) {
		InterfazSesion->ClearOnJoinSessionCompleteDelegate_Handle(ManejadorDelegadoCompletadoUnirSesion);

		// Emitir nuestro propio delegado
		DelegadoMultijugadorCompletadoUnirSesion.Broadcast(EOnJoinSessionCompleteResult::UnknownError);
	}
}

void USubsistemaInstanciaJuego::DestruirSesion() {
	if (!InterfazSesion.IsValid()) {
		DelegadoMultijugadorCompletadoDestruirSesion.Broadcast(false);
		return;
	}

 	ManejadorDelegadoCompletadoDestruirSesion = InterfazSesion->AddOnDestroySessionCompleteDelegate_Handle(DelegadoCompletadoDestruirSesion);

	// Si devuelve true se ejecutar� USubsistemaInstanciaJuego::CallbackCompletadoDestruirSesion 
	if (!InterfazSesion->DestroySession(NAME_GameSession)) {
		InterfazSesion->ClearOnDestroySessionCompleteDelegate_Handle(ManejadorDelegadoCompletadoDestruirSesion);
		DelegadoMultijugadorCompletadoDestruirSesion.Broadcast(false);
	}
}

void USubsistemaInstanciaJuego::EmpezarSesion() {
}

void USubsistemaInstanciaJuego::CallbackCompletadoCrearSesion(FName NombreSesion, bool fueOk) {
	if (InterfazSesion) {
		InterfazSesion->ClearOnCreateSessionCompleteDelegate_Handle(ManejadorDelegadoCompletadoCrearSesion);
	}

	DelegadoMultijugadorCompletadoCrearSesion.Broadcast(fueOk);
}

void USubsistemaInstanciaJuego::CallbackCompletadoEncontrarSesiones(bool fueOk) {
	if (InterfazSesion) {
		InterfazSesion->ClearOnFindSessionsCompleteDelegate_Handle(ManejadorDelegadoCompletadoEncontrarSesiones);
	}

	if (BusquedaUltimaSesion->SearchResults.Num() <= 0) {
		DelegadoMultijugadorCompletadoEncontrarSesiones.Broadcast(TArray<FOnlineSessionSearchResult>(), false);
	} else {
		DelegadoMultijugadorCompletadoEncontrarSesiones.Broadcast(BusquedaUltimaSesion->SearchResults, fueOk);
	}	
}

void USubsistemaInstanciaJuego::CallbackCompletadoUnirSesion(FName NombreSesion, EOnJoinSessionCompleteResult::Type Resultado) {
	if (InterfazSesion) {
		InterfazSesion->ClearOnJoinSessionCompleteDelegate_Handle(ManejadorDelegadoCompletadoUnirSesion);
	}

	DelegadoMultijugadorCompletadoUnirSesion.Broadcast(Resultado);
}

void USubsistemaInstanciaJuego::CallbackCompletadoDestruirSesion(FName NombreSesion, bool fueOk) {
	if (InterfazSesion) {
		InterfazSesion->ClearOnDestroySessionCompleteDelegate_Handle(ManejadorDelegadoCompletadoDestruirSesion);
	}

	if (fueOk && bCrearSesionAlDestruir) {
		bCrearSesionAlDestruir = false;
		CrearSesion(UltimoNumeroConexiones, UltimoModoJuego);
	}

	DelegadoMultijugadorCompletadoDestruirSesion.Broadcast(fueOk);
}

void USubsistemaInstanciaJuego::CallbackCompletadoEmpezarSesion(FName NombreSesion, bool fueOk) {
}
