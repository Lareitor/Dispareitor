#pragma once

#include "CoreMinimal.h"
#include "Proyectil.h"
#include "ProyectilCohete.generated.h"

UCLASS()
class DISPAREITOR_API AProyectilCohete : public AProyectil {
	GENERATED_BODY()

public:
	AProyectilCohete();	
	virtual void Destroyed() override;
	
protected:
	virtual void CallbackAlImpactar(UPrimitiveComponent* ComponenteImpactante, AActor* ActorImpactado, UPrimitiveComponent* ComponenteImpactado, FVector ImpulsoNormal, const FHitResult& ImpactoResultado) override;
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
	USoundCue* SonidoMientraVuela;

	UPROPERTY()
	UAudioComponent* SonidoMientraVuelaComponente;

	UPROPERTY(EditAnywhere)
	USoundAttenuation* SonidoMientraVuelaAtenuacion;

	UPROPERTY(VisibleAnywhere)
	class UCoheteMovimientoComponente* CoheteMovimientoComponente;
};
