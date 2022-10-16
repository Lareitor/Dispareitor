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
    // A partir de ClasePantallaDelPersonaje creamos el objeto PantallaDelPersonaje
	UPROPERTY(EditAnywhere, Category = Estadisticas) TSubclassOf<class UUserWidget> ClasePantallaDelPersonaje; //WBP_PantallaDelPersonaje
	UPROPERTY()	class UPantallaDelPersonaje* PantallaDelPersonaje;
	void MostrarPantallaDelPersonaje();	
	UPROPERTY(EditAnywhere, Category = Anuncios) TSubclassOf<class UUserWidget> ClaseAnunciosWidget; //WBP_Anuncios 
	UPROPERTY() class UAnunciosWidget* AnunciosWidget;
	void MostrarAnunciosWidget();
	UPROPERTY(EditAnywhere, Category = Francotirador) TSubclassOf<class UUserWidget> ClaseFrancotiradorCruceta; //WBP_FrancotiradorCruceta
	UPROPERTY() class UFrancotiradorCruceta* FrancotiradorCruceta;
	void MostrarFrancotiradorCruceta();
	void MostrarAnunciosEliminacion(FString NombreGanador, FString NombrePerdedor);

protected:
	virtual void BeginPlay() override;
	
private:
	UPROPERTY() class ADispareitorControladorJugador* DispareitorControladorJugador; 
	FHUDCruceta HUDCruceta;
	void DibujarCruceta(UTexture2D* Textura, FVector2D CentroPantalla, FVector2D Apertura, FLinearColor ColorCruceta);
	UPROPERTY(EditAnywhere)	float AperturaMaximaCruceta = 16.f;
	UPROPERTY(EditAnywhere) TSubclassOf<class UAnunciosEliminacion> ClaseAnunciosEliminacion;
	UPROPERTY(EditAnywhere) float TiempoAnuncioEliminacion = 10.5f;
	UFUNCTION() void TiempoAnuncioEliminacionFinalizado(UAnunciosEliminacion* MensajeAEliminar);
	UPROPERTY() TArray<UAnunciosEliminacion*> ArrayAnunciosEliminacion;

public:
	FORCEINLINE void ActualizarCrucetaHUD(const FHUDCruceta& HUDT) { HUDCruceta = HUDT; }		
};
