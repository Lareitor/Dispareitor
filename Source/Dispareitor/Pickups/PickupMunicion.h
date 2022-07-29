#pragma once

#include "CoreMinimal.h"
#include "Pickup.h"
#include "Dispareitor/Tipos/TiposArma.h"
#include "PickupMunicion.generated.h"

UCLASS()
class DISPAREITOR_API APickupMunicion : public APickup {
	GENERATED_BODY()
	
protected:
	virtual void Callback_ComponenteEsferaSolapadaInicio(UPrimitiveComponent* ComponenteSolapado, AActor* OtroActor, UPrimitiveComponent* OtroComponente, int32 OtroIndice, bool bFromSweep, const FHitResult& SweepResult);
	
private:
	UPROPERTY(EditAnywhere) int32 IncrementoMunicion = 30;	
	UPROPERTY(EditAnywhere) ETipoArma TipoArma;
};
