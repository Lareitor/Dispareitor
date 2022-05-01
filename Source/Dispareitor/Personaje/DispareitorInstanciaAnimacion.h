#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "DispareitorInstanciaAnimacion.generated.h"

UCLASS()
class DISPAREITOR_API UDispareitorInstanciaAnimacion : public UAnimInstance {
	GENERATED_BODY()

public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaTime) override;

private:
	UPROPERTY(BlueprintReadOnly, Category = Personaje, meta = (AllowPrivateAccess = "true"))
	class ADispareitorPersonaje* DispareitorPersonaje;

	UPROPERTY(BlueprintReadOnly, Category = Movimiento, meta = (AllowPrivateAccess = "true"))
	float Velocidad;

	UPROPERTY(BlueprintReadOnly, Category = Movimiento, meta = (AllowPrivateAccess = "true"))
	bool bEstaEnElAire;

	UPROPERTY(BlueprintReadOnly, Category = Movimiento, meta = (AllowPrivateAccess = "true"))
	bool bEstaAcelerando;

	UPROPERTY(BlueprintReadOnly, Category = Movimiento, meta = (AllowPrivateAccess = "true"))
	bool bArmaEquipada;

	UPROPERTY(BlueprintReadOnly, Category = Movimiento, meta = (AllowPrivateAccess = "true"))
	bool bEstaAgachado;

	UPROPERTY(BlueprintReadOnly, Category = Movimiento, meta = (AllowPrivateAccess = "true"))
	bool bApuntando;

	UPROPERTY(BlueprintReadOnly, Category = Movimiento, meta = (AllowPrivateAccess = "true"))
	float GiroDesviacion;

	UPROPERTY(BlueprintReadOnly, Category = Movimiento, meta = (AllowPrivateAccess = "true"))
	float Inclinacion;

	FRotator PersonajeRotacionUltimoFrame;
	FRotator PersonajeRotacion;
	FRotator DeltaRotacion;

};
