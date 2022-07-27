#pragma once

#include "CoreMinimal.h"
#include "Arma.h"
#include "ArmaProyectil.generated.h"

UCLASS()
class DISPAREITOR_API AArmaProyectil : public AArma {
	GENERATED_BODY()

public:
	virtual void Disparar(const FVector& Objetivo) override;
	
private:
	UPROPERTY(EditAnywhere)	TSubclassOf<class AProyectil> ClaseProyectil;	//rifle: BP_ProyectilBala, lanzagranadas: BP_ProyectilGranada, lanzacohetes: BP_ProyectilCohete
};
