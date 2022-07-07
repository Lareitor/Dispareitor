#pragma once

#include "CoreMinimal.h"
#include "Arma.h"
#include "ArmaHitScan.generated.h"

UCLASS()
class DISPAREITOR_API AArmaHitScan : public AArma {
	GENERATED_BODY()

public:
	virtual void Disparar(const FVector& Objetivo) override;
	
private:
	UPROPERTY(EditAnywhere)
	float Danio = 20.f;	

	UPROPERTY(EditAnywhere)
	class UParticleSystem* ImpactoParticulas;
};
