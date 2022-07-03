#pragma once

#include "CoreMinimal.h"
#include "Proyectil.h"
#include "ProyectilCohete.generated.h"

UCLASS()
class DISPAREITOR_API AProyectilCohete : public AProyectil {
	GENERATED_BODY()

public:
	AProyectilCohete();	
	
protected:
	virtual void CallbackAlImpactar(UPrimitiveComponent* ComponenteImpactante, AActor* ActorImpactado, UPrimitiveComponent* ComponenteImpactado, FVector ImpulsoNormal, const FHitResult& ImpactoResultado) override;

private:
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* Malla;

};
