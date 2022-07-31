#pragma once

#include "CoreMinimal.h"
#include "Pickup.h"
#include "PickupVelocidad.generated.h"

UCLASS()
class DISPAREITOR_API APickupVelocidad : public APickup {
	GENERATED_BODY()

protected:
	virtual void Callback_ComponenteEsferaSolapadaInicio(UPrimitiveComponent* ComponenteSolapado, AActor* OtroActor, UPrimitiveComponent* OtroComponente, int32 OtroIndice, bool bFromSweep, const FHitResult& SweepResult);
	
private:
	UPROPERTY(EditAnywhere) float VelocidadDePie = 1600.f;
	UPROPERTY(EditAnywhere) float VelocidadAgachado = 850.f;
	UPROPERTY(EditAnywhere) float Duracion = 15.f;
};
