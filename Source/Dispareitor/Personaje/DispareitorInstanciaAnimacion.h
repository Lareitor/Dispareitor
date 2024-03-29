#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Dispareitor/Tipos/GirarEnSitio.h"
#include "DispareitorInstanciaAnimacion.generated.h"

UCLASS()
class DISPAREITOR_API UDispareitorInstanciaAnimacion : public UAnimInstance {
	GENERATED_BODY()

public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaTime) override;

private:
	UPROPERTY(BlueprintReadOnly, Category = Personaje, meta = (AllowPrivateAccess = "true")) class ADispareitorPersonaje* DispareitorPersonaje;
	UPROPERTY(BlueprintReadOnly, Category = Movimiento, meta = (AllowPrivateAccess = "true")) float Velocidad;
	UPROPERTY(BlueprintReadOnly, Category = Movimiento, meta = (AllowPrivateAccess = "true")) bool bEnElAire;
	UPROPERTY(BlueprintReadOnly, Category = Movimiento, meta = (AllowPrivateAccess = "true")) bool bAcelerando;
	UPROPERTY(BlueprintReadOnly, Category = Movimiento, meta = (AllowPrivateAccess = "true")) bool bArmaEquipada;
	class AArma* ArmaEquipada;
	UPROPERTY(BlueprintReadOnly, Category = Movimiento, meta = (AllowPrivateAccess = "true")) bool bAgachado;
	UPROPERTY(BlueprintReadOnly, Category = Movimiento, meta = (AllowPrivateAccess = "true")) bool bApuntando;
	UPROPERTY(BlueprintReadOnly, Category = Movimiento, meta = (AllowPrivateAccess = "true")) float GiroDesviacion;
	UPROPERTY(BlueprintReadOnly, Category = Movimiento, meta = (AllowPrivateAccess = "true")) float Inclinacion;
	// AO = Aim Offset
	UPROPERTY(BlueprintReadOnly, Category = Movimiento, meta = (AllowPrivateAccess = "true")) float AOGiro;
	UPROPERTY(BlueprintReadOnly, Category = Movimiento, meta = (AllowPrivateAccess = "true")) float AOInclinacion;
	UPROPERTY(BlueprintReadOnly, Category = Movimiento, meta = (AllowPrivateAccess = "true")) FTransform ManoIzquierdaTransform;
	FRotator RotacionPersonajeAnteriorFrame;
	FRotator RotacionPersonajeActualFrame;
	FRotator RotacionPersonajeDelta;
	UPROPERTY(BlueprintReadOnly, Category = Movimiento, meta = (AllowPrivateAccess = "true")) EGirarEnSitio GirarEnSitio;
	UPROPERTY(BlueprintReadOnly, Category = Movimiento, meta = (AllowPrivateAccess = "true")) FRotator RotacionManoDerecha;
	UPROPERTY(BlueprintReadOnly, Category = Movimiento, meta = (AllowPrivateAccess = "true")) bool bControladoLocalmente;
	UPROPERTY(BlueprintReadOnly, Category = Movimiento, meta = (AllowPrivateAccess = "true")) bool bRotarHuesoRaiz;
	UPROPERTY(BlueprintReadOnly, Category = Movimiento, meta = (AllowPrivateAccess = "true")) bool bEliminado;
	// Para la mano izquierda que usa FABRIK
	UPROPERTY(BlueprintReadOnly, Category = Movimiento, meta = (AllowPrivateAccess = "true")) bool bUsarFABRIK;
	UPROPERTY(BlueprintReadOnly, Category = Movimiento, meta = (AllowPrivateAccess = "true")) bool bTransformarManoDerecha;
	UPROPERTY(BlueprintReadOnly, Category = Movimiento, meta = (AllowPrivateAccess = "true")) bool bUsarAO;

	UPROPERTY(BlueprintReadOnly, Category = Movimiento, meta = (AllowPrivateAccess = "true")) bool bSosteniendoBandera;
};
