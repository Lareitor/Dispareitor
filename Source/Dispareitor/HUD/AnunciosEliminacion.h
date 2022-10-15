#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AnunciosEliminacion.generated.h"

UCLASS()
class DISPAREITOR_API UAnunciosEliminacion : public UUserWidget {
	GENERATED_BODY()
	
public:
	void ActualizarTextoEliminacion(FString NombreGanador, FString NombrePerdedor);
	UPROPERTY(meta = (BindWidget)) class UHorizontalBox* CajaAnunciosEliminacion;
	UPROPERTY(meta = (BindWidget)) class UTextBlock* TextoAnunciosEliminacion;
};
