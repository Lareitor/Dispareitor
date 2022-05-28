#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PantallaDelPersonaje.generated.h"


// Instanciada por DispareitorHUD
UCLASS()
class DISPAREITOR_API UPantallaDelPersonaje : public UUserWidget {
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))	
	class UProgressBar* VidaBarra;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* VidaTexto;
};
