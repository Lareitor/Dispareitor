#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PickupPuntoReaparicion.generated.h"

UCLASS()
class DISPAREITOR_API APickupPuntoReaparicion : public AActor {
	GENERATED_BODY()
	
public:	
	APickupPuntoReaparicion();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;	
	UPROPERTY(EditAnywhere) TArray<TSubclassOf<class APickup>> ClasesPickup;
	UPROPERTY() APickup* PickupReaparecido;
	void ReaparecerPickup();
	void Callback_TemporizadorReaparicionPickupFinalizado();
	UFUNCTION() void IniciarTemporizadorReaparicionPickup(AActor *ActorDestruido); 

private:
	FTimerHandle TemporizadorReaparicionPickup;	
	UPROPERTY(EditAnywhere) float TiempoReaparicionPickupMinimo;
	UPROPERTY(EditAnywhere) float TiempoReaparicionPickupMaximo;
};
