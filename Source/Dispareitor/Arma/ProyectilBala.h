#pragma once

#include "CoreMinimal.h"
#include "Proyectil.h"
#include "ProyectilBala.generated.h"

UCLASS()
class DISPAREITOR_API AProyectilBala : public AProyectil {
	GENERATED_BODY()
public:
	AProyectilBala();

#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& Evento) override;
#endif

protected:
	virtual void Callback_AlImpactar(UPrimitiveComponent* ComponenteImpactante, AActor* ActorImpactado, UPrimitiveComponent* ComponenteImpactado, FVector ImpulsoNormal, const FHitResult& ImpactoResultado) override;
	virtual void BeginPlay() override;
};
