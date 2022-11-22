// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OnlineSessionSettings.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Menu.generated.h"

UCLASS()
class MULTIJUGADORSESIONES_API UMenu : public UUserWidget {
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable) void MenuConfiguracion(int32 _NumeroDeConexiones = 4, FString  _ModoJuego = FString(TEXT("TodosContraTodos")), FString _PathSalaEspera = FString(TEXT("/Game/ThirdPerson/Maps/MapaEspera")));

protected:
	virtual bool Initialize() override;
	virtual void OnLevelRemovedFromWorld(ULevel* Nivel, UWorld* Mundo) override;
	// Callbacks para los delegados creados en SubsistemaInstanciaJuego
	UFUNCTION()	void CallbackCompletadoCrearSesion(bool fueOk);
	void CallbackCompletadoEncontrarSesiones(const TArray<FOnlineSessionSearchResult>& ResultadoBusquedaSesion, bool fueOk);
	void CallbackCompletadoUnirSesion(EOnJoinSessionCompleteResult::Type Resultado);
	UFUNCTION()	void CallbackCompletadoDestruirSesion(bool fueOk);
	UFUNCTION()	void CallbackCompletadoEmpezarSesion(bool fueOk);

private:
	// Con esta propiedad enlazamos el boton de la interfaz grafica con esta variable. Ambos deben tener el mismo nombre
	UPROPERTY(meta = (BindWidget))class UButton* BotonAnfitrion;
	UPROPERTY(meta = (BindWidget)) UButton* BotonUnirse;
	UFUNCTION()	void BotonAnfitrionPulsado();
	UFUNCTION()	void BotonUnirsePulsado();
	void MenuDeshacerConfiguracion();
	class USubsistemaInstanciaJuego* SubsistemaInstanciaJuego;
	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true")) int32 NumeroDeConexiones{4};
	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true")) FString ModoJuego{TEXT("TodosContraTodos")};
	FString PathSalaEspera{ TEXT("") };
};
