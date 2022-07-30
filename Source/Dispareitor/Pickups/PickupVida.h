#pragma once

#include "CoreMinimal.h"
#include "Pickup.h"
#include "PickupVida.generated.h"

UCLASS()
class DISPAREITOR_API APickupVida : public APickup {
	GENERATED_BODY()

public:
	APickupVida();
	virtual void Destroyed() override;

protected:
	virtual void Callback_ComponenteEsferaSolapadaInicio(UPrimitiveComponent* ComponenteSolapado, AActor* OtroActor, UPrimitiveComponent* OtroComponente, int32 OtroIndice, bool bFromSweep, const FHitResult& SweepResult);
	
private:
	UPROPERTY(EditAnywhere) float IncrementoVida = 100.f;	
	UPROPERTY(EditAnywhere) float TiempoIncrementoVida = 5.f;
	UPROPERTY(VisibleAnywhere) class UNiagaraComponent* ComponenteNiagaraVida; // FX
	UPROPERTY(EditAnywhere) class UNiagaraSystem* SistemaNiagaraVida; // FX despues de coger la vida
};
