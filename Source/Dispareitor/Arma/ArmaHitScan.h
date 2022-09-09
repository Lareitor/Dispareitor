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
	FHitResult CalcularImpacto(const FVector& Inicio, const FVector& Objetivo);
	UPROPERTY(EditAnywhere)	class UParticleSystem* SistemaParticulasAlImpactar; //franco: p_impact_metal_large_01, pistola: P_Impact_Metal_Medium_01, subfusil: p_impact_metal_large_01, escopeta: P_Impact_Metal_Small_01
	UPROPERTY(EditAnywhere)	USoundCue* SonidoAlImpactar; //franco: SniperRifle_ImpactSurface_Cue,  subfusil: Rifle_ImpactSurface_Cue, escopeta: Shotgun_ImpactSurface_Cue
	UPROPERTY(EditAnywhere)	float Danio = 20.f;	

private:
	UPROPERTY(EditAnywhere)	UParticleSystem* SistemaParticulasTrazaDeHumo; // franco: P_SmokeTrail, pistola: P_SmokeTrail, subfusil: P_SmokeTrail, escopeta: P_SmokeTrail
	UPROPERTY(EditAnywhere)	UParticleSystem* SistemaParticulasFogonazoEnPuntaArma; // subfusil: P_Pistol_MuzzleFlash_01
	UPROPERTY(EditAnywhere)	USoundCue* SonidoAlDisparar; //subfusil: Subfusil_Cue
};
