#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "DispareitorHUD.generated.h"

USTRUCT(BlueprintType) // Por si lo queremos usar desde BP
struct FHUDTexturas {
	GENERATED_BODY()
public:
	class UTexture2D* CrucetaCentro;
	UTexture2D* CrucetaIzquierda;
	UTexture2D* CrucetaDerecha;
	UTexture2D* CrucetaArriba;
	UTexture2D* CrucetaAbajo;
	float CrucetaApertura;
};

UCLASS()
class DISPAREITOR_API ADispareitorHUD : public AHUD {
	GENERATED_BODY()

public:
	virtual void DrawHUD() override;

private:
	FHUDTexturas HUDTexturas;
	void DibujarCruceta(UTexture2D* Textura, FVector2D PantallaCentro, FVector2D Apertura);

	UPROPERTY(EditAnywhere)
	float CrucetaAperturaMaxima = 16.f;

public:
	FORCEINLINE void ActualizarHUDTexturas(const FHUDTexturas& HUDT) { HUDTexturas = HUDT; }		
	
};