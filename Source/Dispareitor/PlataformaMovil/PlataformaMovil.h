#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/InterpToMovementComponent.h"
#include "PlataformaMovil.generated.h"

UCLASS()
class DISPAREITOR_API APlataformaMovil : public AActor {
	GENERATED_BODY()
	
public:	
	APlataformaMovil();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere) UStaticMeshComponent* ComponenteMallaEstatica;
	UPROPERTY(EditAnywhere) UInterpToMovementComponent* ComponenteInterpolacionMovimiento;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movimiento, Meta = (ExposeOnSpawn = "true", MakeEditWidget = "true", AllowPrivateAccess = "true")) FVector PosicionInicial;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movimiento, Meta = (ExposeOnSpawn = "true", MakeEditWidget = "true", AllowPrivateAccess = "true")) FVector PosicionFinal;
	UPROPERTY(EditAnywhere, Category = Movimiento)	float Duracion = 5.f;	
};
