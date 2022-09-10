#pragma once

#include "CoreMinimal.h"
#include "ArmaHitScan.h"
#include "Escopeta.generated.h"

UCLASS()
class DISPAREITOR_API AEscopeta : public AArmaHitScan {
	GENERATED_BODY()

public:
	virtual void DispararEscopeta(const TArray<FVector_NetQuantize>& Objetivos);
	void CalcularPuntosFinalesConDispersionParaEscopeta(const FVector& Objetivo, TArray<FVector_NetQuantize>& Objetivos);
	
private:
	UPROPERTY(EditAnywhere, Category = "Dispersion") uint32 NumeroPerdigones = 10;	
};
