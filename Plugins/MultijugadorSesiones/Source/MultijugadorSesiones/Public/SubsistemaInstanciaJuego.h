#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "SubsistemaInstanciaJuego.generated.h"

// Declaración de nuestros propios delegados para que la clase Menu pueda enlazarles callbacks
// MULTICAST = Multiples clases puede enlazarse a él
// DYNAMIC = Se puede serializar y puede ser salvado y cargado desde una clase Blueprint. En Blueprint se llaman Event Dispatchers. 
// DYNAMIC implica también que las callbacks que se enlacen deben ser UFUNCTION. En los DYNAMICs separamos el tipo del parametro con la ,
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDelegadoMultijugadorCompletadoCrearSesion, bool, fueOk);
// No es DYNAMIC ya que para que lo fuera todos los argumentos pasados deben ser compatibles con BPs, y FOnlineSessionSearchResult no lo es ya que no es un UFUNCTION
// En los solo MULTICAST no separamos el tipo del parametro
DECLARE_MULTICAST_DELEGATE_TwoParams(FDelegadoMultijugadorCompletadoEncontrarSesiones, const TArray<FOnlineSessionSearchResult>& ResultadoBusquedaSesion, bool fueOk);
DECLARE_MULTICAST_DELEGATE_OneParam(FDelegadoMultijugadorCompletadoUnirSesion, EOnJoinSessionCompleteResult::Type Resultado);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDelegadoMultijugadorCompletadoDestruirSesion, bool, fueOk);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDelegadoMultijugadorCompletadoEmpezarSesion, bool, fueOk);

/**
 * Subsistema para manejar todas las funciones de sesion online
 */
UCLASS()
class MULTIJUGADORSESIONES_API USubsistemaInstanciaJuego : public UGameInstanceSubsystem {
	GENERATED_BODY()
public:
	USubsistemaInstanciaJuego();

	// Llamadas por el proyecto que incluya el plugin
	void CrearSesion(int32 NumeroConexiones, FString ModoJuego);
	void EncontrarSesiones(int32 NumeroMaximoResultados);
	void UnirSesion(const FOnlineSessionSearchResult& ResultadoBusquedaSesion);
	void DestruirSesion();
	void EmpezarSesion();

	// Nuestros propios delegados para que la clase Menu pueda enlazarles callbacks
	FDelegadoMultijugadorCompletadoCrearSesion DelegadoMultijugadorCompletadoCrearSesion;
	FDelegadoMultijugadorCompletadoEncontrarSesiones DelegadoMultijugadorCompletadoEncontrarSesiones;
	FDelegadoMultijugadorCompletadoUnirSesion DelegadoMultijugadorCompletadoUnirSesion;
	FDelegadoMultijugadorCompletadoDestruirSesion DelegadoMultijugadorCompletadoDestruirSesion;
	FDelegadoMultijugadorCompletadoEmpezarSesion DelegadoMultijugadorCompletadoEmpezarSesion;

	int32 NumeroConexionesDeseadas{}; // Inicializado a 0
	FString ModoJuegoDeseado{}; // Inicializado a cadena vacia

protected:
	// Callbacks llamadas por los delegados
	void CallbackCompletadoCrearSesion(FName NombreSesion, bool fueOk);
	void CallbackCompletadoEncontrarSesiones(bool fueOk);
	void CallbackCompletadoUnirSesion(FName NombreSesion, EOnJoinSessionCompleteResult::Type Resultado);
	void CallbackCompletadoDestruirSesion(FName NombreSesion, bool fueOk);
	void CallbackCompletadoEmpezarSesion(FName NombreSesion, bool fueOk);

private:
	IOnlineSessionPtr InterfazSesion;
	TSharedPtr<FOnlineSessionSettings> ConfiguracionUltimaSesion;
	TSharedPtr<FOnlineSessionSearch> BusquedaUltimaSesion;

	// Delegados para añadir a la lista de delegados de la interfaz Online Session
	// A cada uno lo enlazaremos con la funcion callback que han de llamar cuando su acción se realice
	FOnCreateSessionCompleteDelegate DelegadoCompletadoCrearSesion;
	FDelegateHandle ManejadorDelegadoCompletadoCrearSesion;

	FOnFindSessionsCompleteDelegate DelegadoCompletadoEncontrarSesiones;
	FDelegateHandle ManejadorDelegadoCompletadoEncontrarSesiones;

	FOnJoinSessionCompleteDelegate DelegadoCompletadoUnirSesion;
	FDelegateHandle ManejadorDelegadoCompletadoUnirSesion;

	FOnDestroySessionCompleteDelegate DelegadoCompletadoDestruirSesion;
	FDelegateHandle ManejadorDelegadoCompletadoDestruirSesion;

	FOnStartSessionCompleteDelegate DelegadoCompletadoEmpezarSesion;
	FDelegateHandle ManejadorDelegadoCompletadoEmpezarSesion;	

	bool bCrearSesionAlDestruir{ false };
	int32 UltimoNumeroConexiones;
	FString UltimoModoJuego;	
};
