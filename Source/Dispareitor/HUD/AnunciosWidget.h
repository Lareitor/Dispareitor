#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AnunciosWidget.generated.h"

UCLASS()
class DISPAREITOR_API UAnunciosWidget : public UUserWidget {
	GENERATED_BODY()

public:	
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* CalentamientoTiempo;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* PartidaComienza;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Informacion;	
	
};
