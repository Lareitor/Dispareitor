#pragma once

#include "CoreMinimal.h"
#include "Proyectil.h"
#include "ProyectilBala.generated.h"

UCLASS()
class DISPAREITOR_API AProyectilBala : public AProyectil {
	GENERATED_BODY()

protected:
	virtual void CallbackAlImpactar(UPrimitiveComponent* ComponenteImpactante, AActor* ActorImpactado, UPrimitiveComponent* ComponenteImpactado, FVector ImpulsoNormal, const FHitResult& ImpactoResultado) override;
	
};
