#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "DispareitorHUD.generated.h"

USTRUCT(BlueprintType) // Por si lo queremos usar desde BP
struct FHUDCruceta {
	GENERATED_BODY()
public:
	class UTexture2D* CrucetaCentro;
	UTexture2D* CrucetaIzquierda;
	UTexture2D* CrucetaDerecha;
	UTexture2D* CrucetaArriba;
	UTexture2D* CrucetaAbajo;
	float CrucetaApertura;
	FLinearColor CrucetaColor;
};

UCLASS()
class DISPAREITOR_API ADispareitorHUD : public AHUD {
	GENERATED_BODY()

public:
	virtual void DrawHUD() override;

    // A partir de PantallaDelPersonajeClase creamos el objeto PantallaDelPersonaje
	UPROPERTY(EditAnywhere, Category = Estadisticas)
	TSubclassOf<class UUserWidget> PantallaDelPersonajeClase;

	UPROPERTY()
	class UPantallaDelPersonaje* PantallaDelPersonaje;

	void AnadirPantallaDelPersonaje();	

	UPROPERTY(EditAnywhere, Category = Anuncios)
	TSubclassOf<class UUserWidget> AnunciosWidgetClase;

	UPROPERTY()
	class UAnunciosWidget* AnunciosWidget;

	void AnadirAnunciosWidget();

	UPROPERTY(EditAnywhere, Category = Francotirador)
	TSubclassOf<class UUserWidget> FrancotiradorCrucetaClase;

	UPROPERTY()
	class UFrancotiradorCruceta* FrancotiradorCruceta;

	void FrancotiradorCrucetaAnadir();

protected:
	virtual void BeginPlay() override;
	
private:
	FHUDCruceta HUDCruceta;
	void DibujarCruceta(UTexture2D* Textura, FVector2D PantallaCentro, FVector2D Apertura, FLinearColor CrucetaColor);

	UPROPERTY(EditAnywhere)
	float CrucetaAperturaMaxima = 16.f;

public:
	FORCEINLINE void ActualizarHUDCruceta(const FHUDCruceta& HUDT) { HUDCruceta = HUDT; }		
	
};
