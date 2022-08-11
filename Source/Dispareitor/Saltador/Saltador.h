#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMeshActor.h"
#include "Saltador.generated.h"

UCLASS()
class DISPAREITOR_API ASaltador : public AStaticMeshActor {
	GENERATED_BODY()

public: 	
	ASaltador();

protected:
	virtual void BeginPlay() override;
	UFUNCTION()	virtual void Callback_ComponenteCajaSolapadaInicio(UPrimitiveComponent* ComponenteSolapado, AActor* OtroActor, UPrimitiveComponent* OtroComponente, int32 OtroIndice, bool bFromSweep, const FHitResult& SweepResult);
	
private:
	UPROPERTY(EditAnywhere) class UBoxComponent* ComponenteCaja;	
	UPROPERTY(EditAnywhere) float IncrementoSalto = 5000.f;
	UPROPERTY(EditAnywhere) float Duracion = 0.5f;
	
};
