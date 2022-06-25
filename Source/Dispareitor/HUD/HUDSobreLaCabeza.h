#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HUDSobreLaCabeza.generated.h"

UCLASS()
class DISPAREITOR_API UHUDSobreLaCabeza : public UUserWidget {
	GENERATED_BODY()
	
public:
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* TextoSobreCabeza;	

	void ActualizarTextoSobreCabeza(FString texto);

	UFUNCTION(BlueprintCallable)
	void MostrarJugadorRolRed(APawn* Peon);

	UFUNCTION(BlueprintCallable)
	void MostrarJugadorNombre(APawn* Peon);

protected:
	virtual void OnLevelRemovedFromWorld(ULevel *Nivel, UWorld* Mundo) override;	

private:
	bool bActualizadoTextoSobreLaCabeza = false;	
};
