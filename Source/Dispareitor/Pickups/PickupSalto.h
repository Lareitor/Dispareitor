#pragma once

#include "CoreMinimal.h"
#include "Pickup.h"
#include "PickupSalto.generated.h"

UCLASS()
class DISPAREITOR_API APickupSalto : public APickup {
	GENERATED_BODY()

protected:
	virtual void Callback_ComponenteEsferaSolapadaInicio(UPrimitiveComponent* ComponenteSolapado, AActor* OtroActor, UPrimitiveComponent* OtroComponente, int32 OtroIndice, bool bFromSweep, const FHitResult& SweepResult);
		
private:
	UPROPERTY(EditAnywhere) float IncrementoSalto = 2500.f;
	UPROPERTY(EditAnywhere) float Duracion = 15.f;
};
