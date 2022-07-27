#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FrancotiradorCruceta.generated.h"

UCLASS()
class DISPAREITOR_API UFrancotiradorCruceta : public UUserWidget {
	GENERATED_BODY()

public:
	UPROPERTY(Transient, meta = (BindWidgetAnim)) UWidgetAnimation* AnimacionZoomIn;
};
