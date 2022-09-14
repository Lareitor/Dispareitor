#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CompensacionLagComponente.generated.h"

USTRUCT(BlueprintType)
struct FCajaImpacto {
	GENERATED_BODY()

	UPROPERTY()
	FVector Posicion;

	UPROPERTY()
	FRotator Rotacion;

	UPROPERTY()
	FVector CajaExtension;
};

USTRUCT(BlueprintType)
struct FCajasImpactoFrame {
	GENERATED_BODY()

	UPROPERTY()
	float Tiempo;

	UPROPERTY()
	TMap<FName, FCajaImpacto> CajasImpacto;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DISPAREITOR_API UCompensacionLagComponente : public UActorComponent {
	GENERATED_BODY()

public:	
	UCompensacionLagComponente();
	friend class ADispareitorPersonaje;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY() ADispareitorPersonaje* DispareitorPersonaje;	
	UPROPERTY() class ADispareitorControladorJugador* DispareitorControladorJugador;
};
