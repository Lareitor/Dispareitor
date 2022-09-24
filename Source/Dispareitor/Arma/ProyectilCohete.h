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

#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& Evento) override;
#endif
	
protected:
	virtual void Callback_AlImpactar(UPrimitiveComponent* ComponenteImpactante, AActor* ActorImpactado, UPrimitiveComponent* ComponenteImpactado, FVector ImpulsoNormal, const FHitResult& ImpactoResultado) override;
	virtual void BeginPlay() override;
	UPROPERTY(EditAnywhere)	USoundCue* SonidoMientraVuela;
	UPROPERTY()	UAudioComponent* ComponenteSonidoMientraVuela;
	UPROPERTY(EditAnywhere)	USoundAttenuation* AtenuacionSonidoMientraVuela; // RocketLoop_att
	UPROPERTY(VisibleAnywhere) class UCoheteMovimientoComponente* CoheteMovimientoComponente;

private:
	void FinalizarFXDeVuelo(); 
	void IniciarFXDeExplosion();
	bool bHaImpactado = false;
};
