#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Pickup.generated.h"

UCLASS()
class DISPAREITOR_API APickup : public AActor {
	GENERATED_BODY()
	
public:	
	APickup();
	virtual void Tick(float DeltaTime) override;
	virtual void Destroyed() override;

protected:
	virtual void BeginPlay() override;
	UFUNCTION()	virtual void Callback_ComponenteEsferaSolapadaInicio(UPrimitiveComponent* ComponenteSolapado, AActor* OtroActor, UPrimitiveComponent* OtroComponente, int32 OtroIndice, bool bFromSweep, const FHitResult& SweepResult);
	

private:
	UPROPERTY(EditAnywhere) class USphereComponent* ComponenteEsfera;
	UPROPERTY(EditAnywhere) class USoundCue* Sonido;	
	UPROPERTY(EditAnywhere) UStaticMeshComponent* ComponenteMallaEstatica;
};
