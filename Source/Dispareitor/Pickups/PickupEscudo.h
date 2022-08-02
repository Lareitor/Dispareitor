#pragma once

#include "CoreMinimal.h"
#include "Pickup.h"
#include "PickupEscudo.generated.h"

UCLASS()
class DISPAREITOR_API APickupEscudo : public APickup {
	GENERATED_BODY()

protected:
	virtual void Callback_ComponenteEsferaSolapadaInicio(UPrimitiveComponent* ComponenteSolapado, AActor* OtroActor, UPrimitiveComponent* OtroComponente, int32 OtroIndice, bool bFromSweep, const FHitResult& SweepResult);
		
private:
	UPROPERTY(EditAnywhere) float IncrementoEscudo = 100.f;
	UPROPERTY(EditAnywhere) float TiempoIncrementoVida = 5.f;	
	
};
