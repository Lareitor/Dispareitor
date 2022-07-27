#pragma once

#include "CoreMinimal.h"
#include "Proyectil.h"
#include "ProyectilGranada.generated.h"

UCLASS()
class DISPAREITOR_API AProyectilGranada : public AProyectil {
	GENERATED_BODY()

public:
	AProyectilGranada();
	virtual void Destroyed() override;

protected:
	virtual void BeginPlay() override;	
	UFUNCTION()	void Callback_AlRebotar(const FHitResult& ResultadoDelImpacto, const FVector& VelocidadAlImpactar);
	
private:
	UPROPERTY(EditAnywhere)	USoundCue* SonidoAlRebotar;	//GrenadeLauncher_Bounce_Cue
};
