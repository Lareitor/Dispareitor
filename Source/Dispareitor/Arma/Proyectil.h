#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Proyectil.generated.h"

UCLASS()
class DISPAREITOR_API AProyectil : public AActor {
	GENERATED_BODY()
	
public:	
	AProyectil();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere)
	class UBoxComponent* CajaColision;

	UPROPERTY(VisibleAnywhere)
	class UProjectileMovementComponent* ProyectilMovimientoComponente;

	UPROPERTY(EditAnywhere)
	class UParticleSystem* Traza;

	class UParticleSystemComponent* TrazaComponente;

public:	
	

};
