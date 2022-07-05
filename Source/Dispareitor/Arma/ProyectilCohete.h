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
	void DestruirTemporizadorFinalizado();

	UPROPERTY(EditAnywhere)
	class UNiagaraSystem* HumoTraza;

	UPROPERTY()
	class UNiagaraComponent* HumoTrazaComponente; 

	UPROPERTY(EditAnywhere)
	USoundCue* SonidoMientraVuela;

	UPROPERTY()
	UAudioComponent* SonidoMientraVuelaComponente;

	UPROPERTY(EditAnywhere)
	USoundAttenuation* SonidoMientraVuelaAtenuacion;

private:
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* Malla;

	FTimerHandle DestruirTemporizador;

	UPROPERTY(EditAnywhere)
	float DestruirTiempo = 3.f;
};
