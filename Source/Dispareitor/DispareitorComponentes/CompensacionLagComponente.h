#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CompensacionLagComponente.generated.h"

USTRUCT(BlueprintType)
struct FCajaImpacto {
	GENERATED_BODY()

	UPROPERTY()
	FVector Posicion;

	UPROPERTY()
	FRotator Rotacion;

	UPROPERTY()
	FVector CajaExtension;
};

USTRUCT(BlueprintType)
struct FCajasImpactoFrame {
	GENERATED_BODY()

	UPROPERTY()
	float Tiempo;

	UPROPERTY()
	TMap<FName, FCajaImpacto> CajasImpacto;
};


USTRUCT(BlueprintType)
struct FResultadoRebobinarLadoServidor {
	GENERATED_BODY()

	UPROPERTY()
	bool bImpactoConfirmado;

	UPROPERTY()
	bool bTiroALaCabeza;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DISPAREITOR_API UCompensacionLagComponente : public UActorComponent {
	GENERATED_BODY()

public:	
	UCompensacionLagComponente();
	friend class ADispareitorPersonaje;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void MostrarCajasImpactoFrame(const FCajasImpactoFrame& CajasImpactoFrame, const FColor Color);
	FResultadoRebobinarLadoServidor RebobinarLadoServidor(class ADispareitorPersonaje* DispareitorPersonajeImpactado, const FVector_NetQuantize& InicioRayo, const FVector_NetQuantize& ImpactoRayo, float TiempoImpacto);
	UFUNCTION(Server, Reliable) void PeticionImpacto_EnServidor(ADispareitorPersonaje* DispareitorPersonajeImpactado, const FVector_NetQuantize& InicioRayo, const FVector_NetQuantize& ImpactoRayo, float TiempoImpacto, class AArma* ArmaCausanteDanio);

protected:
	virtual void BeginPlay() override;
	void GuardarCajasImpactoFrame(FCajasImpactoFrame& CajasImpactoFrame);
	FCajasImpactoFrame InterpolacionEntreFrames(const FCajasImpactoFrame& CajasImpactoFrameMasJoven, const FCajasImpactoFrame& CajasImpactoFrameMasViejo, float TiempoImpacto);
	FResultadoRebobinarLadoServidor ConfirmarImpacto(const FCajasImpactoFrame& CajasImpactoFrame, ADispareitorPersonaje* DispareitorPersonajeImpactado, const FVector_NetQuantize& InicioRayo, const FVector_NetQuantize& ImpactoRayo);
	void CachearCajasImpactoFrame(ADispareitorPersonaje* DispareitorPersonajeImpactado, FCajasImpactoFrame& CajasImpactoFrameSalida);
	void MoverCajasImpactoFrame(ADispareitorPersonaje* DispareitorPersonajeImpactado,const FCajasImpactoFrame& CajasImpactoFrame);
	void RestaurarCajasImpactoFrame(ADispareitorPersonaje* DispareitorPersonajeImpactado,const FCajasImpactoFrame& CajasImpactoFrame);
	void ModificarColisionMallaPersonaje(ADispareitorPersonaje* DispareitorPersonajeImpactado, ECollisionEnabled::Type TipoColisionPermitida);
	void GuardarCajasImpactoFrame();

private:
	UPROPERTY() ADispareitorPersonaje* DispareitorPersonaje;	
	UPROPERTY() class ADispareitorControladorJugador* DispareitorControladorJugador;
	TDoubleLinkedList<FCajasImpactoFrame> CajasImpactoFrames;
	UPROPERTY(EditAnywhere) float TiempoAlmacenamientoMaximo = 4.f;
};
