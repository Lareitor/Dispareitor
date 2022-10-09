#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RegresarAMenuPrincipal.generated.h"

UCLASS()
class DISPAREITOR_API URegresarAMenuPrincipal : public UUserWidget {
	GENERATED_BODY()

public:
	void ActivarMenu();
	void DesactivarMenu();	

protected:
	virtual bool Initialize() override;
	UFUNCTION() void Callback_AlDestruirSesion(bool bFueOk);
	UFUNCTION() void Callback_AlDejarElJuego();

private:
	UPROPERTY(meta = (BindWidget)) class UButton* BotonMenuPrincipal;	
	UFUNCTION() void PulsadoBotonMenuPrincipal(); 
	UPROPERTY() class USubsistemaInstanciaJuego* SubsistemaInstanciaJuego;
	UPROPERTY() class APlayerController* ControladorJugador;
};
