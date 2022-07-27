#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PantallaDelPersonaje.generated.h"

// Instanciada por DispareitorHUD
UCLASS()
class DISPAREITOR_API UPantallaDelPersonaje : public UUserWidget {
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget)) class UProgressBar* BarraVida;
	UPROPERTY(meta = (BindWidget)) class UTextBlock* TextoVida;
	UPROPERTY(meta = (BindWidget)) UTextBlock* Muertos;
	UPROPERTY(meta = (BindWidget)) UTextBlock* Muertes;
	UPROPERTY(meta = (BindWidget)) UTextBlock* MunicionArma;
	UPROPERTY(meta = (BindWidget)) UTextBlock* MunicionPersonaje;
	UPROPERTY(meta = (BindWidget)) UTextBlock* Tiempo;
	UPROPERTY(meta = (BindWidget)) UTextBlock* CantidadGranadas;
};
