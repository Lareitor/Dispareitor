#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMeshActor.h"
#include "LimitesJuego.generated.h"

UCLASS()
class DISPAREITOR_API ALimitesJuego : public AStaticMeshActor {
	GENERATED_BODY()

public: 
	ALimitesJuego();

protected:
	UFUNCTION()
	virtual void CallbackMallaSolapadoInicio(UPrimitiveComponent* ComponenteSolapado, AActor* OtroActor, UPrimitiveComponent* OtroComponente, int32 OtroIndice, bool bFromSweep, const FHitResult& SweepResult);

	
};
