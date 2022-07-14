#pragma once

#include "CoreMinimal.h"
#include "Arma.h"
#include "ArmaHitScan.generated.h"

UCLASS()
class DISPAREITOR_API AArmaHitScan : public AArma {
	GENERATED_BODY()

public:
	virtual void Disparar(const FVector& Objetivo) override;

protected:
	FVector PuntoFinalConDispersionCalcular(const FVector& PuntoInicial, const FVector& Objetivo);	
	FHitResult ImpactoCalcular(const FVector& Inicio, const FVector& Objetivo);

	UPROPERTY(EditAnywhere)
	class UParticleSystem* ImpactoParticulas;
	
	UPROPERTY(EditAnywhere)
	USoundCue* ImpactoSonido;

	UPROPERTY(EditAnywhere)
	float Danio = 20.f;	

private:
	UPROPERTY(EditAnywhere)
	UParticleSystem* HumoTrazaPS;

	UPROPERTY(EditAnywhere)
	UParticleSystem* PuntaArmaFlash;

	UPROPERTY(EditAnywhere)
	USoundCue* DisparoSonido;

	UPROPERTY(EditAnywhere, Category = "Dispersion")
	float EsferaDistancia = 800.f;

	UPROPERTY(EditAnywhere, Category = "Dispersion")
	float EsferaRadio = 75.f;

	UPROPERTY(EditAnywhere, Category = "Dispersion")
	bool bDispersionUsar = false;
};