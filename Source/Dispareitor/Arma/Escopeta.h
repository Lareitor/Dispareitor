#pragma once

#include "CoreMinimal.h"
#include "ArmaHitScan.h"
#include "Escopeta.generated.h"

UCLASS()
class DISPAREITOR_API AEscopeta : public AArmaHitScan {
	GENERATED_BODY()

public:
	virtual void Disparar(const FVector& Objetivo) override;	
	void CalcularPuntoFinalConDispersionParaEscopeta(const FVector& Objetivo, TArray<FVector>& Objetivos);
	
private:
	UPROPERTY(EditAnywhere, Category = "Dispersion")
	uint32 NumeroPerdigones = 10;	
};
